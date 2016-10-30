<?php

namespace Drupal\less\Form;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\less\Plugin\LessEngineManager;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Class SettingsForm.
 *
 * @package Drupal\less\Form
 */
class SettingsForm extends ConfigFormBase {

  /**
   * LESS Engine Plugin Manager.
   *
   * @var \Drupal\devel\DevelDumperPluginManager
   */
  protected $engineManager;

  /**
   * The instantiated plugin instances that have configuration forms.
   *
   * @var \Drupal\Core\Plugin\PluginFormInterface[]
   */
  protected $configurableInstances = array();

  /**
   * Constructs a new SettingsForm object.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The factory for configuration objects.
   * @param \Drupal\less\Plugin\LessEngineManager $engineManager
   *   Devel Dumper Plugin Manager.
   */
  public function __construct(ConfigFactoryInterface $config_factory, LessEngineManager $engineManager) {
    parent::__construct($config_factory);

    $this->engineManager = $engineManager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('config.factory'),
      $container->get('plugin.manager.less_engine')
    );
  }


  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'settings_form';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['less.settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    /** @var \Drupal\Core\Config\Config $config */
    $config = $this->config('less.settings');

    $form['clear_cache'] = array(
      '#type' => 'details',
      '#title' => t('Clear cache'),
      '#open' => TRUE,
    );

    // TODO: Only clear relevant caches.
    $form['clear_cache']['clear'] = array(
      '#type' => 'submit',
      '#value' => t('Clear all caches'),
      '#submit' => array('::submitCacheClear'),
    );

    $form['engine'] = [
      '#type' => 'radios',
      '#title' => $this->t('LESS engine'),
      '#options' => [],
      '#required' => TRUE,
      '#default_value' => $config->get('engine'),
    ];

    foreach ($this->engineManager->getDefinitions() as $id => $definition) {
      $form['engine']['#options'][$id] = $definition['title'];

      $version = call_user_func([$definition['class'], 'getVersion']);
      $title = $this->t('@title - <a href=":url" target="_blank">:url</a>', [
        '@title' => $definition['title'],
        ':url' => $definition['url']
      ]);

      $form['engine'][$id] = array(
        '#type' => 'radio',
        '#title' => $title,
        '#return_value' => $id,
        '#description' => $definition['description'],
        '#disabled' => empty($version),
      );

      if (!empty($version)) {
        $form['engine'][$id]['#description'] .= ' <b>' . t('Installed version: @version', array('@version' => $version)) . '</b>';
      }
    }

    $is_autoprefixer_installed = FALSE;
    $form['autoprefixer'] = array(
      '#type' => 'checkbox',
      '#title' => $this->t('Use @name - <a href=":url">:url</a>', array('@name' => 'Autoprefixer', ':url' => 'https://github.com/postcss/autoprefixer')),
      '#description' => t('Enable automatic prefixing of vendor CSS extensions.'),
      '#default_value' => $config->get('autoprefixer'),
      '#disabled' => !$is_autoprefixer_installed,
    );

    $form['developer_options'] = array(
      '#type' => 'fieldset',
      '#title' => $this->t('Developer Options'),
      '#tree' => TRUE,
    );

    $form['developer_options']['devel'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('LESS developer mode'),
      '#description' => $this->t('Enable developer mode to ensure LESS files are regenerated every page load.'),
      '#default_value' => $config->get('developer_options.devel'),
    ];

    $form['developer_options']['source_maps'] = array(
      '#type' => 'checkbox',
      '#title' => $this->t('Source Maps'),
      '#description' => $this->t('Enable source maps output while "Developer Mode" is enabled.'),
      '#default_value' => $config->get('developer_options.source_maps'),
      '#states' => array(
        'enabled' => array(
          ':input[name="developer_options[devel]"]' => array('checked' => TRUE),
        ),
      ),
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Submit'),
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    /** @var \Drupal\Core\Config\Config $config */
    $config = $this->config('less.settings');

    $config
      ->set('engine', $form_state->getValue('engine'))
      ->set('autoprefixer', $form_state->getValue('autoprefixer'))
      ->set('developer_options.devel', $form_state->getValue(['developer_options', 'devel']))
      ->set('developer_options.source_maps', $form_state->getValue(['developer_options', 'source_maps']))
      ->save();

    parent::submitForm($form, $form_state);
  }

  /**
   * Clears the caches.
   */
  public function submitCacheClear(array &$form, FormStateInterface $form_state) {
    // TODO: Only clear relevant caches.
    drupal_flush_all_caches();
    drupal_set_message(t('Caches cleared.'));
  }

}
