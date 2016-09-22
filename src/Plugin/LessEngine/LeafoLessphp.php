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
   * @var \lessc
   */
  private $less_php_parser;

  /**
   * Instantiates new instances of \Less_Parser.
   *
   * @inheritdoc
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, $input_file_path) {
    parent::__construct($configuration, $plugin_id, $plugin_definition, $input_file_path);

    $this->less_php_parser = new \lessc();
  }

  /**
   * {@inheritdoc}
   * This compiles using engine specific function calls.
   */
  public function compile() {

    $compiled_styles = NULL;

    try {

      foreach ($this->import_directories as $directory) {
        $this->less_php_parser->addImportDir($directory);
      }

      $cache = $this->less_php_parser->cachedCompile($this->input_file_path);

      $this->dependencies = array_keys($cache['files']);

      $compiled_styles = $cache['compiled'];
    }
    catch (\Exception $e) {

      throw $e;
    }

    return $compiled_styles;
  }

}
