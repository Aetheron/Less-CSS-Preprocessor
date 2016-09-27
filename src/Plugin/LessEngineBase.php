<?php

namespace Drupal\less\Plugin;

use Drupal\Component\Plugin\PluginBase;

/**
 * Base class for Less Engine plugins.
 */
abstract class LessEngineBase extends PluginBase implements LessEngineInterface {

  /**
   * This will get populated with a list of files that $input_file_path depended
   * on through @import statements.
   *
   * @var string[]
   */
  protected $dependencies = array();

  /**
   * This contains any variables that are to be modified into the output.
   *
   * Key => value pairs, where the key is the LESS variable name.
   *
   * @var string[]
   */
  protected $variables = array();

  /**
   * List of directories that are to be used for @import lookups.
   *
   * @var string[]
   */
  protected $import_directories = array();

  /**
   * Flag if source maps are enabled.
   *
   * @var bool
   */
  protected $source_maps_enabled = FALSE;

  /**
   * @var string|NULL
   */
  protected $source_maps_base_path = NULL;

  /**
   * @var string|NULL
   */
  protected $source_maps_root_path = NULL;

  /**
   * {@inheritdoc}
   */
  public function setSource($path) {
    if (empty($path)) {
      throw new \InvalidArgumentException('No source file path given.');
    }

    if (!file_exists($path)) {
      throw new \InvalidArgumentException('Source file does not exist.');
    }

    // Set the source file path.
    $this->configuration['source_path'] = $path;

    // Prepare a relative directory path for the destination file.
    $destination_sub_path = substr($path, 0, -5);  // Remove the '.less' extension.
    if (substr($destination_sub_path, -4) == '.css') {
      $destination_sub_path = substr($destination_sub_path, 0, -4);  // Remove '.css' extension if it exists.
    }

    // Create full path to the destination file.
    $cache_id = \Drupal::state()->get('system.css_js_query_string');
    $destination_uri = 'public://less/' . $cache_id . '/' . $destination_sub_path . '.css';
    $this->configuration['destination_uri'] = $destination_uri;
  }

  /**
   * @inheritdoc
   */
  public function getDestinationUri() {
    return $this->configuration['destination_uri'];
  }

  /**
   * Gets the file to the compiled file as required by \Drupal\Core\Asset\AssetResolver.
   *
   * @see \Drupal\Core\Asset\AssetResolver
   *
   * @inheritdoc
   */
  static public function uriToRelativePath($uri) {
    $file_url = file_create_url($uri);
    $compiled_file_relative_url = file_url_transform_relative($file_url);
    return ltrim($compiled_file_relative_url, '/');
  }

  /**
   * {@inheritdoc}
   */
  public function destinationExists() {
    /** @var \Drupal\Core\File\FileSystemInterface $fileSystem */
    $fileSystem = \Drupal::service('file_system');

    if (file_exists($fileSystem->realpath($this->configuration['destination_uri']))) {
      return TRUE;
    }

    return FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function setImportDirectories(array $directories) {

    $this->import_directories = $directories;
  }

  /**
   * {@inheritdoc}
   */
  public function setSourceMaps($enabled = FALSE, $base_path = NULL, $root_path = NULL) {

    $this->source_maps_enabled = $enabled;
    $this->source_maps_base_path = $base_path;
    $this->source_maps_root_path = $root_path;
  }

  /**
   * {@inheritdoc}
   */
  public function modifyVariables(array $variables) {

    $this->variables = $variables;
  }

  /**
   * {@inheritdoc}
   */
  public function getDependencies() {

    return $this->dependencies;
  }
}
