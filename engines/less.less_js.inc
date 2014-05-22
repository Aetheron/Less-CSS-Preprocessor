<?php

/**
 * @file
 * Libraries integration file for 'less.js' library.
 */


/**
 * Process files using 'less/less.js'.
 */
function _less_lessjs($input_file, $less_settings) {
  
  $lessjs = new Lessjs($input_file);
  
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
