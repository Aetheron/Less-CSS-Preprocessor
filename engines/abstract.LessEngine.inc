<?php

abstract class LessEngine implements LessEngineInterface {

  protected $input_file_path = NULL;

  protected $dependencies = array();

  protected $variables = array();

  /**
   * @var string[]
   */
  protected $import_directories = array();

  /**
   * @var bool
   */
  protected $source_maps_enabled = FALSE;

  public function __construct($input_file_path) {

    $this->input_file_path = $input_file_path;
  }

  public function setImportDirectories(array $directories) {

    $this->import_directories = $directories;
  }

  public function setSourceMaps($enabled = FALSE, $base_path = NULL, $root_path = NULL) {}

  public function modifyVariables(array $variables) {

    $this->variables = $variables;
  }

  public function getDependencies() {

    return $this->dependencies;
  }

  /**
   * @return string
   */
  abstract public function compile();
}
