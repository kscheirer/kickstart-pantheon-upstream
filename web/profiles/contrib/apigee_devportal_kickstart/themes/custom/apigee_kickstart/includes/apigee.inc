<?php

/**
 * @file
 * Theme and preprocess functions for Apigee Edge Entities.
 */

use Drupal\apigee_edge\Entity\AppInterface;

/**
 * Implements template_preprocess_apigee_entity().
 */
function apigee_kickstart_preprocess_apigee_entity(&$variables) {
  /** @var \Drupal\Core\Entity\EntityInterface $entity */
  $entity = $variables['entity'];

  // Use a common preprocess for all apps.
  if ($entity instanceof AppInterface) {
    apigee_kickstart_preprocess_apigee_entity__app($variables);
    return;
  }

  // Entity specific preprocess functions.
  $function = __FUNCTION__ . '__' . $entity->getEntityTypeId();
  if (function_exists($function)) {
    $function($variables);
  }
}

/**
 * Implements template_preprocess_apigee_entity__app().
 */
function apigee_kickstart_preprocess_apigee_entity__app(array &$variables) {
  /** @var \Drupal\apigee_edge\Entity\AppInterface $app */
  $app = $variables['entity'];

  // Add local tasks on the app canonical route as additional tabs.
  $variables['additional_tabs'] = [];

  switch ($app->getEntityTypeId()) {
    case 'team_app':
      $canonical_route_name = 'entity.team_app.canonical';
      $route_parameters = [
        'team' => $app->getAppOwner(),
        'app' => $app->getName(),
      ];
      break;

    default:
      $canonical_route_name = "entity.{$app->getEntityTypeId()}.canonical_by_" . str_replace('_app', '', $app->getEntityTypeId());
      $route_parameters = [
        'user' => $app->getOwnerId(),
        'app' => $app->getName(),
      ];
      break;
  }

  if ($tasks = Drupal::service('plugin.manager.menu.local_task')->getLocalTasks($canonical_route_name)) {
    foreach ($tasks['tabs'] as $tab) {
      $link = $tab['#link'];

      /** @var \Drupal\Core\Url $url */
      $url = $link['url'];
      if ($url->getRouteName() !== $canonical_route_name) {
        $url->setRouteParameters($route_parameters);

        $variables['additional_tabs'][] = [
          'title' => $link['title'],
          'url' => $url,
        ];
      }
    }
  }
}

/**
 * Implement template_preprocess_app_credentials().
 */
function apigee_kickstart_preprocess_app_credential(&$variables) {
  // Render the consumerKey and the consumerSecret as secret fields.
  foreach (['consumerKey', 'consumerSecret'] as $field_name) {
    $variables['primary_wrapper'][$field_name]['value'] = [
      '#type' => 'secret',
      '#value' => $variables['primary_wrapper'][$field_name]['value']['#value'],
    ];
  }

  // Rename title to products.
  $variables['secondary_wrapper']['title']['#title'] = t('Products');
}
