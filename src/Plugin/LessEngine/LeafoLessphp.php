<?php

namespace Drupal\less\Plugin\LessEngine;

use Drupal\Core\Annotation\Translation;
use Drupal\less\Annotation\LessEngine;
use Drupal\less\Plugin\LessEngineBase;


/**
 * Plugin for compiling using the official Leaner CSS CLI.
 *
 * @LessEngine(
 *   id = "leafo_lessphp",
 *   title = @Translation("leafo/lessphp"),
 *   description = @Translation("leafo/lessphp is a compiler for LESS written in PHP."),
 *   url = "https://github.com/leafo/lessphp"
 * )
 */
class LeafoLessphp extends LessEngineBase  {

  /**
   * Instantiates new instances of \Less_Parser.
   *
   * @inheritdoc
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
  }

  /**
   * This compiles using engine specific function calls.
   *
   * {@inheritdoc}
   */
  public function compile() {
    $compiled_styles = NULL;

    $parser = new \lessc();
    try {
      foreach ($this->import_directories as $directory) {
        $parser->addImportDir($directory);
      }

      $parser->setVariables($this->variables);

      $cache = $parser->cachedCompile($this->configuration['source_path']);

      $this->dependencies = array_keys($cache['files']);

      $compiled_styles = $cache['compiled'];
    }
    catch (\Exception $e) {
      throw $e;
    }

    return $compiled_styles;
  }

  /**
   * {@inheritdoc}
   */
  static public function getVersion() {
    if (isset(\lessc::$VERSION)) {
      return \lessc::$VERSION;
    }

    return NULL;
  }
}
