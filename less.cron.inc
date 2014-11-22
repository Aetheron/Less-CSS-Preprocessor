<?php

/**
 * @file
 * Contains functions related to cron for LESS.
 */

/**
 * Implements hook_cron_queue_info().
 * 
 * This hook runs before cache flush during cron. Reliably lets us know if its
 * cron or not.
 */
function less_cron_queue_info() {
  drupal_static('less_cron', TRUE);
}

/**
 * Implements hook_cron().
 * 
 * Removes all stale compiled css files that are no longer in use.
 */
function less_cron() {
  
  file_scan_directory(LESS_DIRECTORY, '/.+/', array('callback' => 'drupal_delete_file_if_stale'));
}
