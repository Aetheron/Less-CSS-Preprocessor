<?php

/**
 * @file
 *   Contains Libraries API integration.
 */

/**
 * Implements hook_libraries_info().
 */
function less_libraries_info() {
  $libraries = array();
  
  $libraries['lessphp'] = array(
    'name' => 'lessphp (Not recommended)',
    'vendor url' => 'http://leafo.net/lessphp/', 
    'download url' => 'http://leafo.net/lessphp/',
    'version arguments' => array(
      'file' => 'lessc.inc.php', 
      'pattern' => '/VERSION\s*=\s*["\']v?([\d\.]+)/', 
      'lines' => 50, 
    ),
    'files' => array(
      'php' => array(
        'lessc.inc.php',
      ),
    ),
  );
  
  _less_lessphp_locate($libraries['lessphp']);
  
  $libraries['less.php'] = array(
    'name' => 'less.php',
    'vendor url' => 'http://lessphp.gpeasy.com/', 
    'download url' => 'http://lessphp.gpeasy.com/#integration-with-other-projects',
    'version arguments' => array(
      'file' => 'Version.php', 
      'pattern' => '/version\s*=\s*["\']([\d\.]+)/', 
      'lines' => 20, 
    ),
    'files' => array(
      'php' => array(
        'Less.php',
      ),
    ),
    'versions' => array(
      '1.7.0' => array(),
    ),
  );
  
  _less_less_php_locate($libraries['less.php']);
  
  $libraries['less.js'] = array(
    'name' => 'less.js',
    'vendor url' => 'http://lesscss.org/', 
    'download url' => 'http://lesscss.org/usage/#using-less-environments',
    'library path' => drupal_get_path('module', 'less'),
    'version callback' => '_less_lessjs_version',
    'files' => array(
      'php' => array(
        'less.class.inc',
      ),
    ),
    'versions' => array(
      '1.5.0' => array(),
    ),
  );
  
  return $libraries;
}

/**
 * Locates oyejorge/less.php in the many possible places it could be.
 * 
 * @param array $library
 *   Libraries definition array.
 */
function _less_less_php_locate(&$library) {
  
  $locations = array();
  
  $locations[] = libraries_get_path('less.php');
  $locations[] = libraries_get_path('lessphp');
  
  $locations[] = drupal_get_path('module', 'less') . '/vendor/oyejorge/less.php';
  
  $version_files = array(
    'lib/Less/Version.php' => 'lessc.inc.php',
    'Version.php' => 'Less.php',
  );
  
  _less_libraries_determine_location($library, $locations, $version_files);
}


/**
 * Locates leafo/lessphp in the many possible places it could be.
 * 
 * @param array $library
 *   Libraries definition array.
 */
function _less_lessphp_locate(&$library) {
  
  $locations = array();
  
  $locations[] = libraries_get_path('lessphp');
  
  $locations[] = drupal_get_path('module', 'less') . '/lessphp';
  $locations[] = drupal_get_path('module', 'less') . '/vendor/leafo/lessphp';
  
  $version_files = array(
    'lessc.inc.php' => 'lessc.inc.php',
  );
  
  _less_libraries_determine_location($library, $locations, $version_files);
}

function _less_libraries_determine_location(&$library, $locations, $version_files) {
  
  foreach ($locations as $location) {
    if ($location) {
      
      foreach ($version_files as $version_file => $class_file) {
        
        if (file_exists($location . DIRECTORY_SEPARATOR . $version_file)) {
          
          $library['library path'] = $location;
          $library['files'] = array(
            'php' => array(
              $class_file,
            ),
          );
          
          $library['version arguments']['file'] = $version_file;
          
          break(2);
        }
      }
    }
  }
}

function _less_lessjs_version() {
  
  $lessjs_version = new lessjs();
  
  return preg_replace('/.*?([\d\.]+).*/', '$1', $lessjs_version->version());
}
