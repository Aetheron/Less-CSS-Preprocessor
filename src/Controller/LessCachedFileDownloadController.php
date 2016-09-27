<?php

namespace Drupal\less\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\less\Plugin\LessEngineBase;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\ProxyClass\Lock\DatabaseLockBackend;
use Drupal\Core\File\FileSystem;
use Drupal\less\Plugin\LessEngineManager;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\HttpKernel\Exception\NotFoundHttpException;

/**
 * Class LessCachedFileDownloadController.
 *
 * @package Drupal\less\Controller
 */
class LessCachedFileDownloadController extends ControllerBase {

  /**
   * Drupal\Core\File\FileSystem definition.
   *
   * @var \Drupal\Core\File\FileSystem
   */
  protected $fileSystem;

  /**
   * Drupal\Core\ProxyClass\Lock\DatabaseLockBackend definition.
   *
   * @var \Drupal\Core\ProxyClass\Lock\DatabaseLockBackend
   */
  protected $lock;

  /**
   * LESS Engine Plugin Manager.
   *
   *
   * @var \Drupal\less\Plugin\LessEngineManager
   */
  protected $engineManager;

  /**
   * Constructs a LessCachedFileDownloadController object.
   *
   * @param \Drupal\Core\File\FileSystemInterface $file_system
   *   Interface for common file system operations.
   * @param \Drupal\Core\Lock\LockBackendInterface $lock
   *   The lock backend.
   * @param \Drupal\less\Plugin\LessEngineManager $engineManager
   *   Less Engine Plugin Manager.
   *
   * {@inheritdoc}
   */
  public function __construct(
    FileSystem $file_system,
    DatabaseLockBackend $lock,
    LessEngineManager $engineManager
  ) {
    $this->lock = $lock;
    $this->fileSystem = $file_system;
    $this->engineManager = $engineManager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('file_system'),
      $container->get('lock'),
      $container->get('plugin.manager.less_engine')
    );
  }

  /**
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The request object.
   * @param string $scheme
   *   The file scheme, defaults to 'private'.
   * @param $cache_id
   *
   * @return Response
   */
  public function deliver(Request $request, $scheme, $cache_id) {
    // Validate the request.
    $css_js_query_string = \Drupal::state()->get('system.css_js_query_string');
    if ($cache_id != $css_js_query_string) {
      throw new NotFoundHttpException();
    }

    // Prepare the destination file URI.
    $file_uri = $scheme . '://less/' . $cache_id . '/' . $request->query->get('file');

    // Get the relative target path.
    $target_path = LessEngineBase::uriToRelativePath($file_uri);

    // Get the map of .less to .css files.
    $map = \Drupal::state()->get('less_css_cache_files') ?: [];
    $source_file_path = array_search($target_path, $map);
    if (empty($source_file_path)) {
      throw new NotFoundHttpException();
    }

    if ($compiled_file_info = _less_process_file($source_file_path, TRUE)) {
      $response = new Response();
      $response->headers->set('Content-Type', 'text/css');
      $response->setContent($compiled_file_info['computed_style']);

      return $response;
    }

    throw new NotFoundHttpException();
  }

}
