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
function language_domain_form_locale_languages_edit_form_alter(&$form, &$form_state) {
  $options = array('' => t('<none>'));
  foreach (domain_domains() as $data) {
    $options[$data['domain_id']] = $data['sitename'];
  }
  $language_domain_map = variable_get('language_domain_map', array());
  $default_value = isset($language_domain_map[$form['langcode']['#value']]) ? $language_domain_map[$form['langcode']['#value']] : '';
  $form['language_domain'] = array(
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
  if (!empty($form_state['values']['language_domain'])) {
    $language_domain_map = variable_get('language_domain_map', array());
    $language_domain_map[$form_state['values']['langcode']] = $form_state['values']['language_domain'];
    variable_set('language_domain_map', $language_domain_map);
  }
}
