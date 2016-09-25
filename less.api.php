<?php

/**
 * @file
 * Hooks provided by the Less module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Define LESS variables.
 *
 * Should return flat associative array, where key is variable name.
 *
 * Variables are lazy evaluated, so variables that depend on others do not have
 * to appear in order.
 *
 * Variables returned by this function are cached, therefore values returned
 * by this function should not change. If you need variables to change from page
 * to page, use hook_less_variables_alter().
 *
 * @return array
 *
 * @see hook_less_variables_alter()
 * @see hook_less_variables_SYSTEM_NAME_alter()
 */
function hook_less_variables() {
  return array(
    '@variable_name_1' => '#ccc',
    '@variable_name_2' => 'darken(@variable_name_1, 30%)',
  );
}

/**
 * Alter LESS variables provided by other modules or themes.
 *
 * This is called before hook_less_variables_SYSTEM_NAME_alter().
 *
 * @param &string[] $less_variables
 *   Flat associative array of variables, where key is variable name.
 *
 * @see hook_less_variables()
 * @see hook_less_variables_SYSTEM_NAME_alter()
 */
function hook_less_variables_alter(array &$variables) {
  $variables['@variable_name_1'] = '#ddd';
}

/**
 * Provide a list of lookup directories for @import statements in .less files.
 *
 * @return string[]
 */
function hook_less_import_directories() {
  return array(
    drupal_get_path('module', 'less_demo') . '/libs',
  );
}

/**
 * Alter LESS include paths.
 *
 * @param &string[]  $import_directories
 */
function hook_less_import_directories_alter(array &$import_directories) {
  $import_directories[] = drupal_get_path('module', 'less_demo') . '/other_path';
}

/**
 * @} End of "addtogroup hooks".
 */
