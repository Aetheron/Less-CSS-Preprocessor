<?php

namespace Drupal\less\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a Less Engine item annotation object.
 *
 * @see \Drupal\less\Plugin\LessEngineManager
 * @see plugin_api
 *
 * @Annotation
 */
class LessEngine extends Plugin {

  /**
   * The plugin ID.
   *
   * @var string
   */
  public $id;

  /**
   * The title of the plugin.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $title;

  /**
   * A URL to the vendor homepage.
   *
   * @var string
   */
  public $url;

}
