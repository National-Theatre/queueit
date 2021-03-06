<?php

/**
 * @file
 * Configuration of the Queue-it module.
 */

/**
 * Form builder.
 */
function queueit_settings_form($form, &$form_state) {
  $known_user = new QueueitKnownUser();

  /* Main configuration */
  $form['config'] = [
    '#title' => 'Queue-it configuration',
    '#type' => 'fieldset',
  ];
  // Implementation type.
  // * Integration - Validate a user via integration config.
  // * JavaScript - Validate a user via JavaScript on the client side.
  // * Code - Specify the configuration in code without using the Trigger/Action paradigm.
  $form['config']['queueit_mode'] = [
    '#type' => 'radios',
    '#title' => t('Queue-it mode'),
    '#options' => [
      'integration' => 'Integration config',
      'js' => 'JavaScript',
      'code' => 'Configuration in code',
    ],
    '#description' => t('Select the method used to integrate Queue-it:<br>Integration Config - used to load the configuration from the Go Queue-it platform.<br>JavaScript - protects pages by the queue on the client side.<br>Configuration in code - recommended for testing specific event or when your application server is not allowed to do external GET requests (this is without using the Trigger/Action paradigm).'),
    '#default_value' => variable_get('queueit_mode', 'integration'),
  ];
  $form['config']['queueit_debug'] = [
    '#type' => 'checkbox',
    '#title' => t('Enable debugging'),
    '#description' => t('Displays Queue-it response on each qualifying page for admin users only.'),
    '#default_value' => variable_get('queueit_debug', FALSE),
  ];
  /* Integration mode */
  $form['integration_config'] = [
    '#title' => t('Integration settings'),
    '#description' => t("Settings related to <i>Integration config</i> mode as defined in Queue-it configuration store and controlled via the Queue-it Go platform. The configuration file consists of Triggers and Actions to determine which pages to protect and which queues to use."),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => variable_get('queueit_mode', 'integration') <> 'integration',
    '#disabled' => variable_get('queueit_mode', 'integration') <> 'integration',
    '#states' => [
      'disabled' => [':input[name="queueit_mode"]' => ['!value' => 'integration']],
      'collapsed' => [':input[name="queueit_mode"]' => ['!value' => 'integration']],
    ]
  ];
  $form['integration_config']['config'] = [
    '#type' => 'item',
    '#prefix' => t('Current configuration:'),
    '#markup' => '<pre>' . $known_user->getIntegrationConfig() . '</pre>',
  ];
  $form['integration_config']['refresh'] = [
    '#type' => 'submit',
    '#value' => t('Refresh'),
    '#executes_submit_callback' => FALSE,
    '#suffix' => sprintf(t('Last update: %s<br>'), strftime("%c", $known_user->getIntegrationConfigTimestamp()))
    . sprintf(t('Endpoint: <a href="%s">%s</a><br>'), $known_user->getIntegrationConfigUrl(), $known_user->getIntegrationConfigUrl())
    . t("<br>Use <i>Refresh</i> button to fetch the latest integration config from Queue-it Go platform. The config is cached with a TTL of 5 mins, after which the new config is loaded on any page load. Clicking the <i>Refresh</i> button more than once every 5 seconds will not trigger another fetch."),
  ];
  /* Configuration using code */
  $form['event_config'] = [
    '#title' => 'Event config',
    '#description' => t('When configuration in code is selected, you can specify the following parameters without using the Trigger/Action paradigm. Useful when your application server is not allowed to do external GET requests.'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => variable_get('queueit_mode', 'integration') <> 'code',
    '#disabled' => variable_get('queueit_mode', 'integration') <> 'code',
    '#states' => [
      'disabled' => [':input[name="queueit_mode"]' => ['!value' => 'code']],
      'collapsed' => [':input[name="queueit_mode"]' => ['!value' => 'code']],
    ]
  ];
  $form['event_config']['queueit_event_id'] = [
    '#type' => 'textfield',
    '#title' => t('Event ID'),
    '#description' => t('Specify ID of the queue to use.'),
    '#default_value' => variable_get('queueit_event_id'),
  ];
  $form['event_config']['queueit_queue_domain'] = [
    '#type' => 'textfield',
    '#title' => t('Existing domain name of the queue - usually in the format [CustomerId].queue-it.net.'),
    '#description' => t('Specify ID of the queue to use.'),
    '#default_value' => variable_get('queueit_queue_domain'),
  ];
  $form['event_config']['queueit_cookie_validity'] = [
    '#type' => 'textfield',
    '#title' => t('Validity of the Queue-it session cookie.'),
    '#description' => t('Optional. Validity of the Queue-it session cookie. Default is 10 minutes.'),
    '#default_value' => variable_get('queueit_cookie_validity'),
    '#element_validate' => ['element_validate_integer_positive'],
  ];
  $form['event_config']['queueit_extend_cookie_validity'] = [
    '#type' => 'checkbox',
    '#title' => t('Extended validity of session cookie.'),
    '#description' => t('Should the Queue-it session cookie validity time be extended each time the validation runs? By default it is enabled.'),
    '#default_value' => variable_get('queueit_extend_cookie_validity', TRUE),
  ];
  $form['event_config']['queueit_layout_name'] = [
    '#type' => 'textfield',
    '#title' => t('Name of the queue ticket layout.'),
    '#description' => t('Optional. E.g. "Default layout by Queue-it. Default is to take what is specified on the Event.'),
    '#default_value' => variable_get('queueit_layout_name'),
  ];
  $form['event_config']['queueit_culture_of_layout'] = [
    '#type' => 'textfield',
    '#title' => t('Culture of the queue ticket layout.'),
    '#description' => t('Culture of the queue ticket layout in the format specified <a href="!url">here</a>.',
      ['!url' => 'https://msdn.microsoft.com/en-us/library/ee825488(v=cs.20).aspx']
    ),
    '#default_value' => variable_get('queueit_culture_of_layout'),
  ];
  /* Validation parameters */
  $form['validate'] = [
    '#title' => 'Validation logic',
    '#type' => 'fieldset',
  ];
  // Exclude Queue-it on specific pages.
  $form['validate']['queueit_exclude_pages'] = [
    '#type' => 'textarea',
    '#title' => t('Exclude Queue-it on specific pages.'),
    '#default_value' => variable_get('queueit_exclude_pages', QUEUEIT_EXCLUDE),
    '#description' => t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard. It is recommended to exclude resources and AJAX calls."),
  ];
  $form['validate']['queueit_ignore_post'] = [
    '#type' => 'checkbox',
    '#title' => t('Ignore queue validation for the POST requests.'),
    '#description' => t('Ignores the KnownUser validation for pages sent via HTTP POST request.'),
    '#default_value' => variable_get('queueit_ignore_post', TRUE),
  ];
  $form['validate']['queueit_ignore_cli'] = [
    '#type' => 'checkbox',
    '#title' => t('Ignore queue validation for the command-line interface.'),
    '#description' => t('Ignores the KnownUser validation when invoked via CLI (such as using drush).'),
    '#default_value' => variable_get('queueit_ignore_cli', TRUE),
  ];
  /* Credentials */
  $form['creds'] = [
    '#title' => 'Queue-it Credentials',
    '#type' => 'fieldset',
  ];
  $form['creds']['queueit_customer_id'] = [
    '#type' => 'textfield',
    '#title' => t('Customer ID'),
    '#description' => t('Your Queue-it customer ID.'),
    '#default_value' => variable_get('queueit_customer_id'),
    '#required' => TRUE,
  ];
  $form['creds']['queueit_api_key'] = [
    '#type' => 'textfield',
    '#title' => t('API Key'),
    '#description' => t('Specify the Api-key which can be supplied through the query string parameter. The key can be found in the GO Queue-it Platform.'),
    '#default_value' => variable_get('queueit_api_key'),
    '#states' => [
      'required' => [':input[name="queueit_mode"]' => ['!value' => 'js']],
    ]
  ];
  $form['creds']['queueit_secret_key'] = [
    '#type' => 'textfield',
    '#title' => t('Secret key'),
    '#description' => t('Your 72 char secret key as specified in Go Queue-it self-service platform.'),
    '#default_value' => variable_get('queueit_secret_key'),
    '#states' => [
      'required' => [':input[name="queueit_mode"]' => ['!value' => 'js']],
    ]
  ];

  return system_settings_form($form);
}

/**
 * Form builder.
 */
function queueit_settings_form_validate($form, &$form_state) {

  // Fail in case of any existing form validation errors.
  if (form_get_errors()) {
    return FALSE;
  }

  // Initialize required variables.
  $int_method = $form_state['values']['queueit_mode'];
  $customer_id = $form_state['values']['queueit_customer_id'];
  $api_key = $form_state['values']['queueit_api_key'];
  $secret_key = $form_state['values']['queueit_secret_key'];

  // Check the backend method requirements.
  if ($int_method <> 'js') {
    module_load_include('install', 'queueit');
    $requirements = module_invoke('queueit', 'requirements', 'runtime');
    $severity = drupal_requirements_severity($requirements);
    if ($severity == REQUIREMENT_ERROR) {
      form_set_error(NULL, 'KnownUser.V3 library not installed!');
      return FALSE;
    }
  }

  // Manually load the file classes.
  module_load_include('php', 'queueit', 'src/classes/QueueitBase');
  module_load_include('php', 'queueit', 'src/classes/QueueitKnownUser');

  // Validate the configuration.
  try {

    switch ($int_method) {
      case 'js':
        $result = QueueitBase::validateJsEndpoints();
        if (!$result) {
          drupal_set_message(t('Cannot access JS endpoints'), 'warning');
        }
        break;

      case 'code':
        $known_user = new QueueitKnownUser($int_method, $customer_id, $secret_key, $api_key);
        if (!$known_user->validateConfig()) {
          drupal_set_message(t('Queue-it configuration is not valid.'), 'warning');
        }
        $result = $known_user->resolveRequestByLocalEventConfig();
        break;

      case 'integration':
        $known_user = new QueueitKnownUser($int_method, $customer_id, $secret_key, $api_key);
        if (!$known_user->validateConfig()) {
          drupal_set_message(t('Queue-it configuration is not valid.'), 'warning');
        }
        $config = $known_user->getIntegrationConfig();
        if (!$config) {
          drupal_set_message(
            t('Cannot fetch the integration configuration from !url.',
            ['!url' => $known_user->getIntegrationConfigUrl()]),
            'warning');
        }
        elseif (!json_decode($config)) {
          drupal_set_message(
            t('Cannot decode the integration configuration from !url.',
            ['!url' => $known_user->getIntegrationConfigUrl()]),
            'warning');
        }
        $result = $known_user->validateRequestByIntegrationConfig();
        break;

      default:
        form_set_error('queueit_mode', $e->getMessage());
        drupal_set_message(
          t('Invalid integration method: !method.',
          ['!method' => $int_method]),
          'error');
    }

  }
  catch (\Exception $e) {
    drupal_set_message(t('Exception error:') . ' ' . $e->getMessage(), 'error');
    watchdog_exception('queueit', $e);
    form_set_error('queueit_mode', $e->getMessage());
  }

  if (!empty($result) && $result) {
    $op = $form_state['triggering_element']['#value'];
    if ($op == 'Refresh' && $known_user->getIntegrationConfig(5)) {
      drupal_set_message(t('Configuration successfully refreshed.'), 'status');
    }
    else {
      drupal_set_message(t('Configuration successfully validated.'), 'status');
    }
  }
  else {
    drupal_set_message(t('Configuration not valid. Please verify your configuration.'), 'warning');
  }
}