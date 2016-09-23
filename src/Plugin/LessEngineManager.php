<?php

namespace Drupal\less\Plugin;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Provides the Less Engine plugin manager.
 */
class LessEngineManager extends DefaultPluginManager {

  /**
   * The config factory.
   *
   * @var \Drupal\Core\Config\ConfigFactoryInterface
   */
  protected $configFactory;


  /**
   * The selected engine.
   *
   * @var \Drupal\less\Plugin\LessEngineInterface
   */
  protected $engine;

  /**
   * Constructor for LessEngineManager objects.
   *
   * @param \Traversable $namespaces
   *   An object that implements \Traversable which contains the root paths
   *   keyed by the corresponding namespace to look for plugin implementations.
   * @param \Drupal\Core\Cache\CacheBackendInterface $cache_backend
   *   Cache backend instance to use.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler to invoke the alter hook with.
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The factory for configuration objects.
   */
  public function __construct(
    \Traversable $namespaces,
    CacheBackendInterface $cache_backend,
    ModuleHandlerInterface $module_handler,
    ConfigFactoryInterface $config_factory
  ) {
    parent::__construct('Plugin/LessEngine', $namespaces, $module_handler, 'Drupal\less\Plugin\LessEngineInterface', 'Drupal\less\Annotation\LessEngine');

    $this->alterInfo('less_less_engine_info');
    $this->setCacheBackend($cache_backend, 'less_less_engine_plugins');
    $this->configFactory = $config_factory;
  }

  /**
   * @param array $options
   *
   * @return \Drupal\less\Plugin\LessEngineInterface
   */
  public function getEngine(array $options) {
    /** @var \Drupal\Core\Config\Config $config */
    $config = $this->configFactory->get('less.settings');

    if (isset($this->engine)) {
      return $this->engine;
    }

    // Load the engine.
    $plugin_id = $config->get('engine');
    if ($plugin_id && $engine = $this->createInstance($plugin_id, $options)) {
      $this->engine = $engine;
    }

    return $this->engine;
  }
}
