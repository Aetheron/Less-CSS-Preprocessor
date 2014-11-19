<?php

class LessEngineLessphp extends LessEngine implements LessEngineInterface {

  /**
   * @var Less_Parser
   */
  private $less_php_parser;

  public function __construct($input_file_path) {

    parent::__construct($input_file_path);

    $this->less_php_parser = new lessc();
  }

  public function setImportDirectories(array $directories) {

    parent::setImportDirectories($directories);

    foreach ($directories as $directory) {
      $this->less_php_parser->addImportDir($directory);
    }
  }

  public function compile() {

    $compiled_styles = NULL;

    try {

      $cache = $this->less_php_parser->cachedCompile($this->input_file_path);

      $this->dependencies = array_keys($cache['files']);

      $compiled_styles = $cache['compiled'];
    }
    catch (Exception $e) {

      throw $e;
    }

    return $compiled_styles;
  }
}
