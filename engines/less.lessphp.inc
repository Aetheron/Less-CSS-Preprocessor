<?php

/**
 * @file
 * Libraries integration file for 'lessphp' library.
 */


/**
 * Process files using 'leafo/lessphp'.
 */
function _less_lessphp($less_settings) {
  
  $input_file = $less_settings['input_file'];
  
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
  
  if (method_exists($less, 'addImportDir')) {
    
    foreach ($less_settings['paths'] as $import_dir) {
      $less->addImportDir($import_dir);
    }
  }
  
  // Try to capture errors.
  try {
    
    if ($less_settings[LESS_DEVEL]) {
      
      $less->setPreserveComments(TRUE);
      $cache = $less->cachedCompile($input_file);
      
      _less_cache_dependencies($input_file, $cache['files']);
      
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
