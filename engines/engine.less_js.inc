<?php

class LessEngineLess_js extends LessEngine implements LessEngineInterface {

  /**
   * @var Less_Parser
   */
  private $less_js_parser;

  public function __construct($input_file_path) {

    parent::__construct($input_file_path);

    $this->less_js_parser = new Lessjs($input_file_path);
  }

  public function setImportDirectories(array $directories) {

    parent::setImportDirectories($directories);

    foreach ($directories as $directory) {
      $this->less_js_parser->include_path($directory);
    }
  }

  public function setSourceMaps($enabled = FALSE, $base_path = '/', $root_path = NULL) {

    $this->source_maps_enabled = $enabled;

    $this->less_js_parser->source_maps($enabled, $root_path, $base_path);
  }

  public function modifyVariables(array $variables) {

    parent::modifyVariables($variables);

    foreach ($variables as $var_name => $var_value) {
      $this->less_js_parser->modify_var(trim($var_name, '@'), trim($var_value, ';'));
    }
  }

  public function getDependencies() {

    $this->dependencies = $this->less_js_parser->depends();

    return parent::getDependencies();
  }

  public function compile() {

    $compiled_styles = NULL;

    try {

      $compiled_styles = $this->less_js_parser->compile();

      $error = $this->less_js_parser->get_error();

      if (!empty($error)) {

        throw new Exception($error);
      }
    }
    catch (Exception $e) {

      throw $e;
    }

    return $compiled_styles;
  }
}
