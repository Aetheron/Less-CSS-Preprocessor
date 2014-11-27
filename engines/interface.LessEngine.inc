<?php

interface LessEngineInterface {

  /**
   * @param array $directories
   */
  public function setImportDirectories(array $directories);

  /**
   * @param bool $enabled
   *   Set the source maps flag.
   * @param string $base_path
   *   Leading value to be stripped from each source map URL.
   *   @link http://lesscss.org/usage/#command-line-usage-source-map-basepath
   * @param string $root_path
   *   Value to be prepended to each source map URL.
   *   @link http://lesscss.org/usage/#command-line-usage-source-map-rootpath
   */
  public function setSourceMaps($enabled, $base_path, $root_path);

  /**
   * @param array $variables
   */
  public function modifyVariables(array $variables);

  /**
   * @return array
   */
  public function getDependencies();

  /**
   * @return string
   */
  public function compile();
}
