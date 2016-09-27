<?php

namespace Drupal\less\Routing;

use Drupal\Core\DependencyInjection\ContainerInjectionInterface;
use Drupal\Core\StreamWrapper\StreamWrapperManagerInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\Routing\Route;

/**
 * Defines a route subscriber to register a url for serving compiled less files.
 */
class LessCachedFileRoutes implements ContainerInjectionInterface {

  /**
   * The stream wrapper manager service.
   *
   * @var \Drupal\Core\StreamWrapper\StreamWrapperManagerInterface
   */
  protected $streamWrapperManager;

  /**
   * Constructs a new PathProcessorImageStyles object.
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
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('stream_wrapper_manager')
    );
  }

  /**
   * Returns an array of route objects.
   *
   * @return \Symfony\Component\Routing\Route[]
   *   An array of route objects.
   */
  public function routes() {
    $routes = array();

    $directory_path = $this->streamWrapperManager->getViaScheme('public')->getDirectoryPath();

    // If clean URLs are enabled and the compiled file already exists, PHP will be bypassed.
    $routes['less.cached_file_public'] = new Route(
      '/' . $directory_path . '/less/{cache_id}/{scheme}',
      array(
        '_controller' => '\Drupal\less\Controller\LessCachedFileDownloadController::deliver',
      ),
      array(
        '_access' => 'TRUE',
      )
    );
    return $routes;
  }

}
