<?php

namespace Drupal\less\PathProcessor;

use Drupal\Core\PathProcessor\InboundPathProcessorInterface;
use Drupal\Core\StreamWrapper\StreamWrapperManagerInterface;
use Symfony\Component\HttpFoundation\Request;

/**
 * Defines a path processor to rewrite URLs for LESS cache files..
 *
 * As the route system does not allow arbitrary amount of parameters convert
 * the file path to a query parameter on the request.
 *
 * Inspired from:
 * @see \Drupal\image\PathProcessor\PathProcessorImageStyles
 */
class PathProcessorLessCachedFiles implements InboundPathProcessorInterface {

  /**
   * The stream wrapper manager service.
   *
   * @var \Drupal\Core\StreamWrapper\StreamWrapperManagerInterface
   */
  protected $streamWrapperManager;

  /**
   * Constructs a new PathProcessorLessCachedFiles object.
   *
   * @param \Drupal\Core\StreamWrapper\StreamWrapperManagerInterface $stream_wrapper_manager
   *   The stream wrapper manager service.
   */
  public function __construct(StreamWrapperManagerInterface $stream_wrapper_manager) {
    $this->streamWrapperManager = $stream_wrapper_manager;
  }

  /**
   * {@inheritdoc}
   */
  public function processInbound($path, Request $request) {
    /** @noinspection PhpUndefinedMethodInspection */
    $directory_path = $this->streamWrapperManager->getViaScheme('public')->getDirectoryPath();

    if (strpos($path, '/' . $directory_path . '/less/') === 0) {
      $path_prefix = '/' . $directory_path . '/less/';
      $scheme = 'public';
    }
    elseif (strpos($path, '/system/files/less/') === 0) {
      $path_prefix = '/system/files/less/';
      $scheme = 'private';
    }
    else {
      return $path;
    }

    // Strip out path prefix.
    $rest = preg_replace('|^' . preg_quote($path_prefix, '|') . '|', '', $path);

    // Provide the requested file path to:
    // \Drupal\less\Controller\LessCachedFileDownloadController.
    if (substr_count($rest, '/') >= 1) {
      list($cache_id, $file) = explode('/', $rest, 2);

      // Set the file as query parameter.
      $request->query->set('file', $file);

      return $path_prefix . $cache_id . '/' . $scheme;
    }
    else {
      return $path;
    }
  }

}
