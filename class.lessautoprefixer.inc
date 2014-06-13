<?php

/**
 * @file
 * Contains 'LessAutoprefixer' class; an abstraction layer for command line Autoprefixer.
 */

/**
 * 'Autoprefixer' class.
 */
class LessAutoprefixer {
  
  // Base command is hardcoded here to reduce security vulnerability.
  private static $base_command = 'autoprefixer';
  
  protected $input_file = NULL;
  
  protected $source_maps_enabled = FALSE;
  
  // Holds value for current error.
  protected $error = NULL;
  
  /**
   * Constructor function for 'LessAutoprefixer'.
   * 
   * @param string $input_file
   *   Path for .less file relative to getcwd().
   */
  public function __construct($input_file = NULL) {
    
    $this->input_file = $input_file;
  }
  
  /**
   * Returns the version string from command line Autoprefixer.
   * 
   * @return string
   *   Version string from Autoprefixer.
   */
  public function version() {
    
    return $this->proc_open(array('--version'));
  }
  
  /**
   * Returns the current error if one exists.
   * 
   * @return string
   *   Error string returned by command line Autoprefixer.
   */
  public function get_error() {
    return $this->error;
  }
  
  /**
   * Enable source maps for current file, and configure source map paths.
   * 
   * @param bool $enabled
   *   Set the source maps flag.
   */
  public function source_maps($enabled) {
    $this->source_maps_enabled = $enabled;
  }
  
  /**
   * Provides list to command line arguments for execution.
   * 
   * @return array
   *   Array of command line arguments.
   */
  private function command_arguments() {
    
    $arguments = array();
    
    // Set service map flags.
    if ($this->source_maps_enabled) {
      
      $arguments[] = '--map';
      $arguments[] = '--inline-map';
    }
    
    // Input file should be last argument.
    $arguments[] = $this->input_file;
    
    return $arguments;
  }
  
  /**
   * Executes auto-prefixing of LESS output file.
   * 
   * @return string
   *   Compiled CSS.
   */
  public function compile() {
    
    return $this->proc_open($this->command_arguments());
  }
  
  private function proc_open($command_arguments = array(), $stdin = NULL) {
    
    $output_data = NULL;
    
    $command = implode(' ', array_merge(array(self::$base_command), $command_arguments));
    
    // Handles for data exchange.
    $pipes = array(
      0 => NULL, // STDIN
      1 => NULL, // STDOUT
      2 => NULL, // STDERR
    );
    
    // Sets permissions on $pipes.
    $descriptors = array(
      0 => array('pipe', 'r'), // STDIN
      1 => array('pipe', 'w'), // STDOUT
      2 => array('pipe', 'w'), // STDERR
    );
    
    $process = proc_open($command, $descriptors, $pipes);
    
    if (is_resource($process)) {
      
      // STDIN not currently used in LESS module.
      if ($this->input_file === '-' && isset($stdin)) {
        fwrite($pipes[0], $stdin);
      }
      fclose($pipes[0]); // fclose() on STDIN executes $command, if program is expecting input from STDIN.
      
      $output_data = stream_get_contents($pipes[1]);
      fclose($pipes[1]);
      
      $this->error = stream_get_contents($pipes[2]);
      fclose($pipes[2]);
      
      proc_close($process);
    }
    
    return $output_data;
  }
}
