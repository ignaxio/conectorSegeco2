<?php

function conectorSegeco2_new_server_admin() { 
    $servidores      = variable_get('codeserver_configs');
    $nombreServidores = array();
    foreach ($servidores as $nombre => $datos) {
        $nombreServidores[$nombre] = $nombre;
    }    
   
    $form['conectorSegeco2_titulo2'] = array(
        '#markup'       => '<h3>' . t('Crea un nuevo servidor.') . '</h3>'
    );
    $form['conectorSegeco2_name'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Nombre del servidor'),
        '#required'         => FALSE,
    );
    $form['conectorSegeco2_serverUrl'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Dirección del servidor'),
        '#required'         => FALSE,
    );
    $form['conectorSegeco2_encrypt_key'] = array(
        '#type'             => 'textfield',
        '#title'            => t('La clave de encriptación'),
        '#required'         => FALSE,
    );
    $form['conectorSegeco2_encrypt_iv'] = array(
        '#type'             => 'textfield',
        '#title'            => t('La clave de enctiptación iv'),
        '#required'         => FALSE,
    );
    $form['conectorSegeco2_ip_nueva'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Inserta una ip nueva'),
        '#required'         => FALSE,
    );
    $form['conectorSegeco2_funcion_nueva'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Inserta una función nueva'),
        '#required'         => FALSE,
    );

    return system_settings_form($form);
}

function conectorSegeco2_new_server_admin_validate($form, &$form_state) {
    $configs                = variable_get('codeserver_configs');
    $nombreDelServidor      = $form_state['values']['conectorSegeco2_name'];
    $DireccionDelServidor   = $form_state['values']['conectorSegeco2_serverUrl'];
    $encryptKey             = $form_state['values']['conectorSegeco2_encrypt_key'];
    $encryptIv              = $form_state['values']['conectorSegeco2_encrypt_iv'];
    $ipNueva                = $form_state['values']['conectorSegeco2_ip_nueva'];    
    $funcionNueva           = $form_state['values']['conectorSegeco2_funcion_nueva'];
        
    //Vamos a construit el array de configuración del servidor.
    $configs[$nombreDelServidor] = array(
        'server' => $DireccionDelServidor,
        //'encrypt_key' => openssl_random_pseudo_bytes(16),
        'encrypt_key' => $encryptKey,
        //'encrypt_iv' => openssl_random_pseudo_bytes(16),
        'encrypt_iv' => $encryptIv,
        'allowed_ips' => array_unique(array($ipNueva)),
        'allowed_functions' => array($funcionNueva),
    );
    
    // Save the updated configs.    
    variable_set('codeserver_configs', $configs);
    drupal_flush_all_caches();
    drupal_set_message(t('Configuración guardada correctamente para el ' . $nombreDelServidor));    
}
