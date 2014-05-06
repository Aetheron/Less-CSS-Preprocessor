<?php

/**
 * @file
 * Contains the administration pages for LESS.
 *
 */

function less_settings_form($form, &$form_state) {

  $form['less_flush'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#value' => t('Click this button regenerate all LESS files once.'),
  );

  $form['less_flush']['flush'] = array(
    '#type' => 'submit',
    '#submit' => array('_flush_less'),
    '#value' => t('Flush LESS files'),
  );
  
  $less_compiler_links = array(
    '!less_php' => l('oyejorge/less.php', 'http://lessphp.gpeasy.com/'),
    '!lessphp' => l('leafo/lessphp', 'http://leafo.net/lessphp/'),
    '!less_js' => l('less/less.js', 'http://lesscss.org/'),
  );
  
  $form['less_engine'] = array(
    '#type' => 'radios',
    '#title' => t('LESS engine'),
    '#options' => array(
      'less.php' => t('less.php - !less_php', $less_compiler_links),
      'less.js' => t('less.js - !less_js', $less_compiler_links),
      'lessphp' => t('lessphp - !lessphp', $less_compiler_links),
    ),
    '#required' => TRUE,
    '#default_value' => variable_get('less_engine', NULL),
  );
  
  _less_inc();
  
  /*
   * Individual 'radio' elements to control disable state.
   */
  $form['less_engine']['less.php'] = array(
    '#type' => 'radio',
    '#title' => $form['less_engine']['#options']['less.php'],
    '#return_value' => 'less.php',
    '#description' => class_exists('Less_Version') ? check_plain(Less_Version::version) : '',
  );
  
  $form['less_engine']['lessphp'] = array(
    '#type' => 'radio',
    '#title' => $form['less_engine']['#options']['lessphp'],
    '#return_value' => 'lessphp',
    '#description' => class_exists('lessc') ? check_plain(lessc::$VERSION) : '',
  );
  
  $form['less_engine']['less.js'] = array(
    '#type' => 'radio',
    '#title' => $form['less_engine']['#options']['less.js'],
    '#return_value' => 'less.js',
    '#description' => check_plain(exec('lessc --version')),
  );
  
  foreach (element_children($form['less_engine']) as $key) {
    $form['less_engine'][$key]['#disabled'] = empty($form['less_engine'][$key]['#description']);
  }
  
  if (empty($form['less_engine']['less.php']['#disabled'])) {
    $form['less_engine']['lessphp']['#title'] .= t(' (compatibility mode through !less_php)', $less_compiler_links);
  }
  
  $form['less_devel'] = array(
    '#type' => 'checkbox',
    '#title' => t('LESS developer mode'),
    '#description' => t('Enable developer mode to ensure LESS files are regenerated every page load.'),
    '#default_value' => variable_get('less_devel', FALSE),
  );
  
  $form['less_watch'] = array(
    '#type' => 'checkbox',
    '#title' => t('LESS watch mode'),
    '#description' => t('Enable watch mode while developer mode is active to automatically reload styles when changes are detected, including changes to @import-ed files. Does not cause a page reload.'),
    '#default_value' => variable_get('less_watch', TRUE),
    '#states' => array(
      'enabled' => array(
        ':input[name="less_devel"]' => array('checked' => TRUE),
      ),
    ),
  );
  
  $form['#submit'] = array('less_settings_form_submit');
  
  return system_settings_form($form);
}

function less_settings_form_submit($form, &$form_state) {
  cache_clear_all('less:', 'cache', TRUE);
}

function _flush_less($form, &$form_state) {

  _less_get_dir(TRUE);
  cache_clear_all('less:', 'cache', TRUE);

  drupal_set_message(t('LESS files cache cleared.'), 'status');
}
