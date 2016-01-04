<?php

function conectorSegeco2_server_config_admin($form, &$form_state) {   
    $nombreDelServidor = $form_state['build_info']['args'][0];
    $funcionesActivas   = array();
    $ipActivas          = array();
    $form               = array();
    $configuracion      = variable_get('codeserver_configs');
    $configuracion      = $configuracion[$nombreDelServidor];            
            
    //Voy a sacar las funciones que hay activas en el módulo conectorSegeco2    
    foreach ($configuracion['allowed_functions'] as $funcion) {
        $funcionesActivas[$funcion] = $funcion;
    }
    //Las ip´s permitidas
    foreach ($configuracion['allowed_ips'] as $ip) {
        $ipActivas[$ip] = $ip;
    }
    
    $form['descripcion'] = array(
        '#markup'       => '<h3>' . t('Página de configuración del servidor.') . '</h3><p>' . t('Si quieres eliminar el servidor deja en blanco el nombre.') . '</p>'
    );
    $form['conectorSegeco2_name'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Nombre del servidor'),
        '#default_value'    => $nombreDelServidor, 
        '#required'         => FALSE,
    );
    $form['conectorSegeco2_serverUrl'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Dirección del servidor'),
        '#default_value'    => $configuracion['server'], 
        '#required'         => FALSE,
    );
    $form['conectorSegeco2_encrypt_key'] = array(
        '#type'             => 'textfield',
        '#title'            => t('La clave de encriptación'),
        '#default_value'    => $configuracion['encrypt_key'], 
        '#required'         => FALSE,
    );
    $form['conectorSegeco2_encrypt_iv'] = array(
        '#type'             => 'textfield',
        '#title'            => t('La clave de enctiptación iv'),
        '#default_value'    => $configuracion['encrypt_iv'], 
        '#required'         => FALSE,
    );
    $form['conectorSegeco2_allowed_ips'] = array(
        '#type'             => 'checkboxes',
        '#options'          => drupal_map_assoc($ipActivas),
        '#title'            => t('¿Qué ip´s de clientes quieres que estén activas para poder hacer peticiones?'),
        '#attributes'       => array('checked' => array('')),
    );
    $form['conectorSegeco2_ip_nueva'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Inserta una ip nueva'),
        '#required'         => FALSE,
    );
    $form['conectorSegeco2_funciones_activas'] = array(
        '#type'             => 'checkboxes',
        '#options'          => drupal_map_assoc($funcionesActivas),
        '#title'            => t('¿Qué funciones quieres que estén activas?'),
        '#attributes'       => array('checked' => array('')),
    );
    $form['conectorSegeco2_funcion_nueva'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Inserta una función nueva'),
        '#required'         => FALSE,
    );
    return system_settings_form($form);
}

function conectorSegeco2_server_config_admin_validate($form, &$form_state) {
    $configs                = array();
    $funcionesNuevas        = array();
    $ipsNuevas              = array();
    $nombreDelServidor      = $form_state['values']['conectorSegeco2_name'];
    $DireccionDelServidor   = $form_state['values']['conectorSegeco2_serverUrl'];
    $encryptKey             = $form_state['values']['conectorSegeco2_encrypt_key'];
    $encryptIv              = $form_state['values']['conectorSegeco2_encrypt_iv'];
    $allowedIps             = $form_state['values']['conectorSegeco2_allowed_ips'];
    $ipNueva                = $form_state['values']['conectorSegeco2_ip_nueva'];    
    $funcionesActivas       = $form_state['values']['conectorSegeco2_funciones_activas'];
    $funcionNueva           = $form_state['values']['conectorSegeco2_funcion_nueva'];   
    
    //Aquí es donde recogemos las funciones que se han mandado y las guardamos en la base de datos.
    //primero vamos a ver si se ha quitado alguna función que se haya desmarcado de las ya habilitadas
    foreach ($funcionesActivas as $funcion) {
        //Si la $key es igual a 0 tenemos que eliminar del array que mandemos al final.
        if($funcion) $funcionesNuevas[] = $funcion;
    }
    //Ahora miramos si ha metido una nueva y la añadimos al array.
    if($funcionNueva != '') $funcionesNuevas[] = $funcionNueva;
    
    //Ahora con las ip`s
    foreach ($allowedIps as $ip) {
        //Si la $key es igual a 0 tenemos que eliminar del array que mandemos al final.
        if($ip) $ipsNuevas[] = $ip;
    }
    //Ahora miramos si ha metido una nueva y la añadimos al array.
    if($ipNueva != '') $ipsNuevas[] = $ipNueva;
    
    //Vamos a construit el array de configuración del servidor.
    $configs[$nombreDelServidor] = array(
        'server' => $DireccionDelServidor,
        //'encrypt_key' => openssl_random_pseudo_bytes(16),
        'encrypt_key' => $encryptKey,
        //'encrypt_iv' => openssl_random_pseudo_bytes(16),
        'encrypt_iv' => $encryptIv,
        'allowed_ips' => array_unique($ipsNuevas),
        'allowed_functions' => $funcionesNuevas,
    );
    // Save the updated configs.    
    variable_set('codeserver_configs', $configs);
    drupal_set_message(t('Configuración guardada correctamente para el ' . $nombreDelServidor));
    
}
