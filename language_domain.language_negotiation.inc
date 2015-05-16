<?php

/**
 * @file
 * Contains custom callbacks for the "locale-url" language negotiation method.
 */

// We only override some callback. Load the others that are used by default.
require_once DRUPAL_ROOT . '/includes/locale.inc';

/**
 * Custom "language" callback for the "locale-url" language negotiation method.
 *
 * @see hook_language_negotiation_info()
 * @see locale_language_negotiation_info()
 * @see language_domain_language_negotiation_info_alter()
 * @see locale_language_from_url()
 */
function language_domain_locale_language_from_url($languages) {
  if (!language_negotiation_get_any(LOCALE_LANGUAGE_NEGOTIATION_URL)) {
    return FALSE;
  }

  if (variable_get('locale_language_negotiation_url_part', LOCALE_LANGUAGE_NEGOTIATION_URL_PREFIX) == LOCALE_LANGUAGE_NEGOTIATION_URL_PREFIX) {
    list($language, $path) = language_domain_language_url_split_prefix(isset($_GET['q']) ? $_GET['q'] : '', $languages);
    if ($language !== FALSE) {
      $_GET['q'] = $path;
      return $language->language;
    }
    return FALSE;
  }

  return locale_language_from_url($languages);
}
