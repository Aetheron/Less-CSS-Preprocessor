<?php

namespace Drupal\less\Plugin\LessEngine;

use Drupal\Core\Annotation\Translation;
use Drupal\less\Annotation\LessEngine;
use Drupal\less\LessCliWrapper;
use Drupal\less\Plugin\LessEngineBase;


/**
 * Plugin for compiling using the official Leaner CSS CLI.
 *
 * @LessEngine(
 *   id = "less_less_js",
 *   title = @Translation("less/less.js"),
 *   description = @Translation("Convert files using the official lesscss.org JavaScript CLI."),
 *   url = "https://github.com/less/less.js"
 * )
 */
class LessLessJs extends LessEngineBase  {

  /**
   * @var \Drupal\less\LessCliWrapper
   */
  private $less_js_parser;

  /**
   * Instantiates new instances of \Lessjs.
   * @see \Lessjs
   *
   * @inheritdoc
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, $input_file_path) {
    parent::__construct($configuration, $plugin_id, $plugin_definition, $input_file_path);

    $this->less_js_parser = LessCliWrapper::create($this->input_file_path);
  }

  /**
   * We override here because getting dependencies from less.js requires another
   * full parse. This way we only do that if dependencies are requested.
   *
   * @return string[]
   *
   * @see \Lessjs::depends()
   */
  public function getDependencies() {

    $this->dependencies = $this->less_js_parser->depends();

    return parent::getDependencies();
  }

  /**
   * {@inheritdoc}
   * This compiles using engine specific function calls.
   */
  public function compile() {

    $compiled_styles = NULL;

    try {

      $this->less_js_parser->source_maps($this->source_maps_enabled, $this->source_maps_base_path, $this->source_maps_root_path);

      foreach ($this->import_directories as $directory) {
        $this->less_js_parser->include_path($directory);
      }

      foreach ($this->variables as $var_name => $var_value) {
        $this->less_js_parser->modify_var(trim($var_name, '@'), trim($var_value, ';'));
      }

      $compiled_styles = $this->less_js_parser->compile();
    }
    catch (\Exception $e) {

      throw $e;
    }

    return $compiled_styles;
  }
}
