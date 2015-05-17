# Language Domain

A Drupal module that allows attaching domains to languages.

The dependencies are locale and domain. However the module is designed to be used with language_fallback and redirector.

The **URL** language detection method should be the only one enabled at `admin/config/regional/language/configure` to make this module work.

## Features

- Allows to attach domains to languages. This means that the active domain would be selected depending on the currently active language.
- Improves the **URL** language detection method to work properly with language path prefixes containing slashes.
