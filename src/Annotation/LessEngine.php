<?php

namespace Drupal\less\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a LESS Engine item annotation object.
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
   * The description of the plugin.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $description;

  /**
   * The URL of the vendor homepage.
   *
   * @var string
   */
  public $url;

}
