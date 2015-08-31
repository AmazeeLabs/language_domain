<?php

/**
 * @file
 * Contains language configuration support code.
 */

/**
 * Implements hook_preprocess_HOOK().
 */
function language_domain_preprocess_table(&$vars) {

  // Detect language overview table.
  if (isset($vars['attributes']['id']) && $vars['attributes']['id'] == 'language-order') {
    $languages = language_list();
    $domains = domain_domains();
    $language_domain_map = variable_get('language_domain_map', array());
    $vars['header'][0]['data'] = t('Name for admins');
    $vars['header'][1]['data'] = t('Name for visitors');
    array_splice($vars['header'], 3, 0, array(
      array('data' => t('Path prefix')),
      array('data' => t('Attached domain')),
    ));
    foreach ($vars['rows'] as &$row) {
      $language = $languages[$row['data'][2]];
      $domain_id = isset($language_domain_map[$language->language]) ? $language_domain_map[$language->language] : NULL;
      $domain_name = $domain_id && isset($domains[$domain_id]) ? $domains[$domain_id]['sitename'] : '';
      array_splice($row['data'], 3, 0, array(
        check_plain($language->prefix),
        check_plain($domain_name),
      ));
    }
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function language_domain_form_locale_languages_overview_form_alter(&$form, &$form_state) {

  // Detect the language_fallback 7.x-2.x version.
  if (
    module_exists('language_fallback')
    && ($modules = system_list('module_enabled'))
    && isset($modules['language_fallback']->schema_version)
    && $modules['language_fallback']->schema_version >= 7200
  ) {

    // Add the graph visualizing the language fallback.
    $languages = language_list();
    $chains = db_select('language_fallback', 'lf')
      ->fields('lf', array('language', 'chain'))
      ->execute()
      ->fetchAllKeyed();
    $links = array();
    foreach ($chains as $langcode => $chain) {
      $chain = explode('|', $chain);
      $source = $langcode;
      foreach ($chain as $target) {
        $link = array(
          'source' => $languages[$source]->name,
          'target' => $languages[$target]->name,
        );
        if (!in_array($link, $links, TRUE)) {
          $links[] = $link;
        }
        $source = $target;
      }
    }
    $form['#attached']['js'][] = array(
      'type' => 'setting',
      'data' => array(
        'language_fallback_overview_links' => $links,
      ),
    );
    $form['#attached']['css'][] = drupal_get_path('module', 'language_domain') . '/css/language_fallback_overview.css';
    $form['#attached']['js']['http://d3js.org/d3.v3.js'] = array(
      'type' => 'external',
    );
    $form['#attached']['js'][] = drupal_get_path('module', 'language_domain') . '/js/language_fallback_overview.js';
    $form['actions']['#suffix'] = '<br /><div style="text-align: center;">' . t('Language fallback overview') . '</div><div id="language-fallback-overview-graph"></div>';
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function language_domain_form_locale_languages_edit_form_alter(&$form, &$form_state) {
  _language_domain_alter_language_form($form, $form);
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function language_domain_form_locale_languages_custom_form_alter(&$form, &$form_state) {
  _language_domain_alter_language_form($form, $form['custom language']);
}

/**
 * Alters add/edit language forms, adds "Attached domain" selector.
 *
 * @param array $form
 *   The whole form.
 * @param array $language_element
 *   The part of the form containing language property elements.
 */
function _language_domain_alter_language_form(&$form, &$language_element) {
  $language_element['name']['#title'] = t('Name for admins');
  $language_element['name']['#description'] = t('This name will be visible for admins only.');
  $language_element['native']['#title'] = t('Name for visitors');
  $language_element['native']['#description'] = t('This name will be visible in the language switcher.');

  $options = array('' => t('<none>'));
  foreach (domain_domains() as $data) {
    $options[$data['domain_id']] = $data['sitename'];
  }
  $language_domain_map = variable_get('language_domain_map', array());
  $default_value = isset($language_element['langcode']['#value']) && isset($language_domain_map[$language_element['langcode']['#value']])
    ? $language_domain_map[$language_element['langcode']['#value']]
    : '';
  $language_element['language_domain'] = array(
    '#type' => 'select',
    '#title' => t('Attached domain'),
    '#options' => $options,
    '#description' => t('When this language is active, the selected domain will be set as active as well.
<br /><strong>WARNING</strong>: this only works if <strong>URL</strong> is the only <a href="!language_config_url" target="_blank">detection method</a>! It works this way because the <a href="!language_domain_url" target="_blank">Language Domain module</a> providing this feature is designed to work in conjunction with the <a href="!redirector_url" target="_blank">Redirector module<a/>.', array(
      '!language_config_url' => url('admin/config/regional/language/configure'),
      '!language_domain_url' => 'https://github.com/AmazeeLabs/language_domain',
      '!redirector_url' => 'https://github.com/AmazeeLabs/redirector',
    )),
    '#default_value' => $default_value,
  );

  $form['#submit'][] = 'language_domain_languages_edit_form_submit';
}

/**
 * Additional submit handler for the "locale_languages_edit_form" form.
 */
function language_domain_languages_edit_form_submit(&$form, &$form_state) {
  if (isset($form_state['values']['language_domain'])) {
    $language_domain_map = variable_get('language_domain_map', array());
    if (empty($form_state['values']['language_domain'])) {
      unset($language_domain_map[$form_state['values']['langcode']]);
    }
    else {
      $language_domain_map[$form_state['values']['langcode']] = $form_state['values']['language_domain'];
    }
    variable_set('language_domain_map', $language_domain_map);
  }
}
