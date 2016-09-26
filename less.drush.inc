<?php

/**
 * @file
 * Contains Drush related functions.
 */

/**
 * Implements hook_drush_cache_clear().
 *
 * This adds an option on drush 'cache-clear'.
 *
 * @inheritdoc
 */
function less_drush_cache_clear(&$types, $include_bootstrapped_types) {
  $types['less'] = 'less_clear_css_cache_files';
}
