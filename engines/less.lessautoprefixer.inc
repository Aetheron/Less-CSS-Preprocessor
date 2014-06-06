<?php


function _less_lessautopreifxer($less_settings) {
  
  $prefixer = new LessAutoprefixer(drupal_realpath($less_settings['output_file']));
      
  return array(
    $prefixer->compile(),
    $prefixer->get_error(),
  );
}
