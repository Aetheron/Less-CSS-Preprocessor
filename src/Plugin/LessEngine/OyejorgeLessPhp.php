<?php

namespace Drupal\less\Plugin\LessEngine;

use Drupal\Core\Annotation\Translation;
use Drupal\less\Annotation\LessEngine;
use Drupal\less\Plugin\LessEngineBase;


/**
 * Plugin for compiling using the official Leaner CSS CLI.
 *
 * @LessEngine(
 *   id = "oyejorge_less_php",
 *   title = @Translation("oyejorge/less.php"),
 *   description = @Translation("A PHP port that is closer to canonical spec from lesscss.org."),
 *   url = "https://github.com/oyejorge/less.php"
 * )
 */
class OyejorgeLessPhp extends LessEngineBase  {

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

    $parser = new \Less_Parser();
    try {

      if ($this->source_maps_enabled) {

        $parser->SetOption('sourceMap', $this->source_maps_enabled);

        $parser->SetOption('sourceMapBasepath', $this->source_maps_base_path);
        $parser->SetOption('sourceMapRootpath', $this->source_maps_root_path);
      }

      // Less.js does not allow path aliasing. Set aliases to blank for consistency.
      $parser->SetImportDirs(array_fill_keys($this->import_directories, ''));

      $parser->parseFile($this->configuration['source_path']);

      $parser->ModifyVars($this->variables);

      $compiled_styles = $parser->getCss();

      $this->dependencies = $parser->AllParsedFiles();
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
    if (class_exists('\Less_Version')) {
      return \Less_Version::version;
    }

    return NULL;
  }

}
