<?php

/**
 * @file
 *   Contains functions related to compiling .less files.
 */

function _less_process_file(&$item) {
  
  $input_file = $item['data'];
  
  $output_file = $item['less']['output_file'];
  
  // $output_file doesn't exist or is flagged for build.
  if (!is_file($output_file) || !empty($item['less']['build_required'])) {
    
    $output_data = NULL;
    $error = NULL;
    
    switch (_less_inc()) {
      case 'less.php':
        list($output_data, $error) = _less_less_php($item['less']);
        break;
      case 'lessphp':
        list($output_data, $error) = _less_lessphp($item['less']);
        break;
      case 'less.js':
        list($output_data, $error) = _less_lessjs($item['less']);
        break;
      default:
        $error = t('No engine found.');
        break;
    }
    
    if (!empty($error)) {
      
      $message_vars = array('@message' => $error, '%input_file' => $input_file);
      
      watchdog('LESS', 'LESS error: @message, %input_file', $message_vars, WATCHDOG_ERROR);
      
      if (user_access(LESS_PERMISSION)) {
        drupal_set_message(t('LESS error: @message, %input_file', $message_vars), 'error');
      }
    }
    
    if (isset($output_data)) {
      
      // Fix paths for images as .css is in different location.
      $output_data = _less_rewrite_paths($input_file, $output_data);
      
      file_unmanaged_save_data($output_data, $output_file, FILE_EXISTS_REPLACE);
    }
  }

  if (is_file($output_file)) {
    
    // Set render path of the stylesheet to the compiled output.
    $item['data'] = $output_file;
  }

}

/**
 * Normalize keeping track of changed files.
 * 
 * @param string $input_file
 *   Path of source file.
 * @param array $dependencies
 *   Array of files that are @import'ed in $input_file, recursively.
 */
function _less_cache_dependencies($input_file, $dependencies = array()) {
  
  // Add $input_file to $dependencies as it is not in return from engines.
  $dependencies = array_merge(array($input_file), (array) $dependencies);
  
  $watched_files = array();
  
  foreach ($dependencies as $dependency) {
    
    // Full path on file should enforce uniqueness in associative array.
    $watched_files[realpath($dependency)] = filemtime($dependency);
  }
  
  cache_set('less:devel:' . drupal_hash_base64($input_file), $watched_files);
}


function _less_output_path(&$item) {
  
  $input_file = $item['less']['input_file'];
  
  $file_uri = file_uri_target($input_file);
  $output_directory = 'public://less/' . _less_get_dir() . '/' . dirname($file_uri ? $file_uri : $input_file);
  
  // Ensure the destination directory exists.
  if (_less_ensure_directory($output_directory)) {
    
    $basename = basename($input_file, '.less');
    
    // Strip '.css' extension of filenames following the RTL extension pattern.
    $basename = basename($basename, '.css');
    
    $less_settings = $item['less'];
    
    // array_multisort() the data so that the hash returns the same hash regardless order of data.
    array_multisort($less_settings);
    
    // json_encode() is used because serialize() throws an error with lambda functions.
    $item['less']['output_file'] = $output_directory . '/' . $basename . '.' . drupal_hash_base64(json_encode($less_settings)) . '.css';
  }
}

function _less_attach_settings(&$item) {
  
  $defaults = array(
    'less' => less_get_settings(), // Bare defaults for LESS.
  );
  
  // These items must be reset for consistent operation.
  $nullify = array(
    'less' => array(
      'input_file' => NULL,
      'output_file' => NULL,
      'build_required' => NULL,
    ),
  );
  
  // Merge in any info from $item.
  $item = array_replace_recursive($defaults, $item, $nullify);
  
  $less_settings = less_get_settings(_less_file_owner($item['data']));
  
  // array_replace_recursive() works on keys, flip to not use numeric keys.
  $less_settings['paths'] = array_flip($less_settings['paths']);
  $item['less']['paths'] = array_flip($item['less']['paths']);
  
  // Merge defaults with any per file settings.
  $item['less'] = array_replace_recursive($less_settings, $item['less']);
  
  // First array_flips before merge removed duplicates, so just flip back.
  $item['less']['paths'] = array_flip($item['less']['paths']);
  
  $item['less']['input_file'] = $item['data'];
}

function _less_check_build(&$item) {
  
  $input_file = $item['less']['input_file'];
  $output_file = $item['less']['output_file'];
  
  $build_required = FALSE;
  
  // Set $rebuild if this file or its children have been modified.
  if ($less_file_cache = cache_get('less:devel:' . drupal_hash_base64($input_file))) {
    
    // Iterate over each file and check if there are any changes.
    foreach ($less_file_cache->data as $filepath => $filemtime) {
      
      // Only rebuild if there has been a change to a file.
      if (is_file($filepath) && filemtime($filepath) > $filemtime) {
        $build_required = TRUE;
        break;
      }
    }
  }
  else {
    
    // No cache data, force a rebuild for later comparison.
    $build_required = TRUE;
  }
  
  $item['less']['build_required'] = $build_required;
}

function _less_store_cache_info(&$item) {
  
  // Only match when output_file exists.
  if ($item['data'] === $item['less']['output_file']) {
    
    global $theme;
    
    $less_watch_cache = $item;
    
    $less_watch_cache['data'] = $item['less']['input_file'];
    
    cache_set('less:watch:' . drupal_hash_base64(file_create_url($item['less']['output_file'])), $less_watch_cache);
    
    // 'preprocess' being FALSE generates a discreet <link /> rather than an @import.
    $item['preprocess'] = FALSE;
  }
}
