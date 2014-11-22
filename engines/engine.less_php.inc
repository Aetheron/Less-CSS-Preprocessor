<?php

class LessEngineLess_php extends LessEngine implements LessEngineInterface {

  /**
   * @var Less_Parser
   */
  private $less_php_parser;

  public function __construct($input_file_path) {

    parent::__construct($input_file_path);

    $this->less_php_parser = new Less_Parser();
  }

  public function setImportDirectories(array $directories) {

    parent::setImportDirectories($directories);

    // Less.js does not allow path aliasing. Set aliases to blank for consistency.
    $this->less_php_parser->SetImportDirs(array_fill_keys($directories, ''));
  }

  public function setSourceMaps($enabled = FALSE, $base_path = NULL, $root_path  = '/') {

    $this->source_maps_enabled = $enabled;

    $this->less_php_parser->SetOption('sourceMap', $enabled);

    $this->less_php_parser->SetOption('sourceMapBasepath', $base_path);
    $this->less_php_parser->SetOption('sourceMapRootpath', $root_path);
  }

  public function getDependencies() {

    $this->dependencies = $this->less_php_parser->AllParsedFiles();

    return parent::getDependencies();
  }

  public function compile() {

    $compiled_styles = NULL;

    try {

      $this->less_php_parser->parseFile($this->input_file_path);

      $this->less_php_parser->ModifyVars($this->variables);

      $compiled_styles = $this->less_php_parser->getCss();
    }
    catch (Exception $e) {

      throw $e;
    }

    return $compiled_styles;
  }
}
