<?php

/**
 * @file
 *   Libraries integration file for 'less.php' library.
 */


/**
 * Processes files using 'oyejorge/less.php'.
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
