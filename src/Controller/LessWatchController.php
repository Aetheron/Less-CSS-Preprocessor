<?php

namespace Drupal\less\Controller;

use Drupal\Component\Utility\Crypt;
use Drupal\Component\Utility\UrlHelper;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\JsonResponse;
use Symfony\Component\HttpFoundation\Request;
use Drupal\less\Plugin\LessEngineManager;

/**
 * Class LessWatchController.
 *
 * @package Drupal\less\Controller
 */
class LessWatchController extends ControllerBase {

  /**
   * LESS Engine Plugin Manager.
   *
   * @var \Drupal\less\Plugin\LessEngineManager
   */
  protected $engineManager;

  /**
   * The config factory.
   *
   * @var \Drupal\Core\Config\ConfigFactoryInterface
   */
  protected $configFactory;

  /**
   * The current request.
   *
   * @var \Symfony\Component\HttpFoundation\Request
   */
  protected $request;

  /**
   * The cache bin.
   *
   * @var \Drupal\Core\Cache\CacheBackendInterface.
   */
  protected $cache;

  /**
   * @param \Drupal\less\Plugin\LessEngineManager $engineManager
   *   Devel Dumper Plugin Manager.
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The factory for configuration objects.
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The current request.
   * @param \Drupal\Core\Cache\CacheBackendInterface $cache
   *   The cache bin.
   */
  public function __construct(LessEngineManager $engineManager, ConfigFactoryInterface $config_factory, Request $request, CacheBackendInterface $cache) {
    $this->engineManager = $engineManager;
    $this->configFactory = $config_factory;
    $this->request = $request;
    $this->cache = $cache;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('plugin.manager.less_engine'),
      $container->get('config.factory'),
      $container->get('request_stack')->getCurrentRequest(),
      $container->get('cache.render')
    );
  }

  /**
   * Hello.
   *
   * @return string
   *   Return Hello string.
   */
  public function watch() {
    global $theme;

    /** @var \Drupal\Core\Config\Config $config */
    $config = $this->config('less.settings');

    $changed_files = array();

    if ($config->get('developer_options.watch_mode')) {

      $files = $this->request->get('less_files', []);

      foreach ($files as $file) {
        $file_url_parts = UrlHelper::parse($file);

        $cid = 'less:watch:' . Crypt::hashBase64($file_url_parts['path']);
        if ($cache = $this->cache->get($cid)) {

          $cached_data = $cache->data;

          $input_file = $cached_data['less']['input_file'];

          $output_file = $cached_data['less']['output_file'];

          $current_mtime = filemtime($output_file);

          $theme = $cached_data['less']['theme'];

          $styles = array(
            '#items' => array(
              $input_file => $cached_data,
            ),
          );

          $styles = _less_pre_render($styles);

          if (filemtime($styles['#items'][$input_file]['data']) > $current_mtime) {
            $changed_files[] = array(
              'old_file' => $file_url_parts['path'],
              'new_file' => file_create_url($styles['#items'][$input_file]['data']),
            );
          }
        }
      }
    }

    return new JsonResponse($changed_files);
  }

}
