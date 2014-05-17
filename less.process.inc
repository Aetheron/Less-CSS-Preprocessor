<?php

/**
 * @file
 *   Contains functions related to compiling .less files.
 */

function _less_process_file(&$info) {
  
  $input_file = $info['data'];
  
  $less_devel = variable_get('less_devel', FALSE);
  $less_dir = _less_get_dir();
  
  $less_path = 'public://less/' . $less_dir;
  
  $file_uri = file_uri_target($input_file);
  $css_path = $less_path . '/' . dirname($file_uri ? $file_uri : $input_file);
  
  // Ensure the destination directory exists.
  if (!_less_ensure_directory($css_path)) {
    // Cancel the for each loop when directory doesn't exist.
    break;
  }

  $output_file = $css_path . '/' . basename($input_file, '.less');
  // Correct file names of files not following the .css.less naming convention
  if (drupal_substr($output_file, -4) != '.css') {
    $output_file .= '.css';
  }
  
  $less_settings = less_get_settings(_less_file_owner($input_file));
  
  if (!empty($info['less'])) {
    $less_settings = drupal_array_merge_deep($less_settings, (array) $info['less']);
  }
  
  $less_settings['less_devel'] = $less_devel;
  
  // array_multisort() the data so that the hash returns the same hash regardless order of data.
  array_multisort($less_settings);
  
  // json_encode() is used because serialize() throws an error with lambda functions.
  $output_file = substr_replace($output_file, drupal_hash_base64(json_encode($less_settings)) . '.css', -3);
  
  $rebuild = FALSE;
  
  // Set $rebuild if this file or its children have been modified.
  if ($less_devel && is_file($output_file)) {
    $output_file_mtime = filemtime($output_file);
    
    if ($less_file_cache = cache_get('less:devel:' . drupal_hash_base64($input_file))) {
      
      // Iterate over each file and check if there are any changes.
      foreach ($less_file_cache->data as $filepath => $filemtime) {
        
        // Only rebuild if there has been a change to a file.
        if (is_file($filepath) && filemtime($filepath) > $filemtime) {
          $rebuild = TRUE;
          break;
        }
      }
    }
    else {
      
      // No cache data, force a rebuild for later comparison.
      $rebuild = TRUE;
    }
  }
  
  // $output_file doesn't exist or is flagged for rebuild.
  if ((!is_file($output_file) || $rebuild) && _less_inc()) {
    
    $output_data = NULL;
    $error = NULL;
    
    switch (variable_get('less_engine', 'lessphp')) {
      case 'less.php':
        list($output_data, $error) = _less_less_php($input_file, $less_settings);
        break;
      case 'lessphp':
        list($output_data, $error) = _less_lessphp($input_file, $less_settings);
        break;
      case 'less.js':
        list($output_data, $error) = _less_lessjs($input_file, $less_settings);
        break;
      default:
        $error = t('No engine found.');
        break;
    }
    
    if ($error) {
      
      $message = 'LESS error: @message, %input_file';
      $message_vars = array('@message' => $error, '%input_file' => $input_file);
      
      watchdog('LESS', $message, $message_vars, WATCHDOG_ERROR);
      
      if (user_access(LESS_PERMISSION)) {
        drupal_set_message(t($message, $message_vars), 'error');
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
    $info['data'] = $output_file;
    
    if ($less_devel) {
      
      $less_watch_cache = $info;
      
      $less_watch_cache['less'] = $less_settings;
      $less_watch_cache['data'] = $input_file;
      $less_watch_cache['output_file'] = $output_file;

      cache_set('less:watch:' . drupal_hash_base64(file_create_url($output_file)), $less_watch_cache);
      
      // 'preprocess' being FALSE generates a discreet <link /> rather than an @import.
      $info['preprocess'] = FALSE;
    }
  }

}



/**
 * Processes files using oyejorge/less.php
 */
function _less_less_php($input_file, $less_settings) {
  
  $output_data = NULL;
  $error = NULL;
  
  $parser = new Less_Parser();
  
  $parser->ModifyVars($less_settings['variables']);
  
  $import_dirs = array();
  
  foreach ($less_settings['paths'] as $path) {
    $import_dirs[$path] = '';
  }
  
  $parser->SetImportDirs($import_dirs);
  
  // Capture errors
  try {
    
    if ($less_settings['less_devel']) {
      
      $parsed_files = Less_Parser::AllParsedFiles();
      
      $file_times = array();
      
      foreach ($parsed_files as $parsed_file) {
        $file_times[realpath($parsed_file)] = filemtime($parsed_file);
      }
      
      cache_set('less:devel:' . drupal_hash_base64($input_file), $file_times);
      
      $parser->SetOption('sourceMap', TRUE);
      // $parser->SetOption('outputSourceFiles', TRUE); // Might not need this.
      $parser->SetOption('sourceMapRootpath', '/'); // @link https://github.com/oyejorge/less.php/issues/127
      $parser->SetOption('sourceMapBasepath', DRUPAL_ROOT); // @link https://github.com/oyejorge/less.php/issues/127
    }

    $parser->parseFile(realpath($input_file));
    
    $output_data = $parser->getCss();
  }
  catch (Exception $e) {
    
    $error = $e->getMessage();
  }
  
  return array($output_data, $error);
}

/**
 * Process files using leafo/lessphp
 */
function _less_lessphp($input_file, $less_settings) {
  
  $output_data = NULL;
  $error = NULL;
  
  $less = new lessc();
  
  if (method_exists($less, 'registerFunction') && is_array($less_settings['functions'])) {
    foreach ($less_settings['functions'] as $funcion => $callback) {
      $less->registerFunction($funcion, $callback);
    }
  }
  
  if (method_exists($less, 'setVariables')) {
    $less->setVariables($less_settings['variables']);
  }
  
  // LESS variables compatibility for less.php alternate library.
  if (class_exists('Less_Version')) {
    
    // Make so imports work from original file directory.
    $less->addImportDir(dirname($input_file));
  }
  
  // Capture errors
  try {
    
    if ($less_settings['less_devel']) {
      
      $less->setPreserveComments(true);
      $cache = $less->cachedCompile($input_file);
      
      cache_set('less:devel:' . drupal_hash_base64($input_file), $cache['files']);
      
      $output_data = $cache['compiled'];
    }
    else {
      
      $output_data = $less->compileFile($input_file);
    }
  }
  catch (Exception $e) {
    
    $error = $e->getMessage();
  }
  
  return array($output_data, $error);
}

function _less_lessjs($input_file, $less_settings) {
  
  $lessjs = new lessjs($input_file);
  
  foreach ($less_settings['variables'] as $var_name => $var_value) {
    $lessjs->global_var(trim($var_name, '@'), trim($var_value, ';'));
  }
  
  foreach ($less_settings['paths'] as $include_path) {
    $lessjs->include_path($include_path);
  }
  
  if ($less_settings['less_devel']) {
    
    $lessjs->source_maps(TRUE, $GLOBALS['base_path'], DRUPAL_ROOT);
    
    $watched_files = $lessjs->depends();
    
    // Add $input_file to watch list as it is not in return from '--depends'.
    array_unshift($watched_files, realpath($input_file));
    
    $watched_files_cache = array();
    
    foreach ($watched_files as $watched_file) {
      $watched_files_cache[$watched_file] = filemtime($watched_file);
    }
    
    cache_set('less:devel:' . drupal_hash_base64($input_file), $watched_files_cache);
  }
  
  return array(
    $lessjs->compile(),
    $lessjs->get_error(),
  );
}
