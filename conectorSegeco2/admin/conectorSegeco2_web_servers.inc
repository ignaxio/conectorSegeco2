<?php

function conectorSegeco2_web_servers_admin() {
    $header = array(t('Server name'), t('Server path'), t('Allowed functions'), t('Edit'));
    $rows = array();
    $funcionesActivas = '';
    $config = variable_get('codeserver_configs');

    foreach ($config as $nombre => $server) {
        $count = 1;
        //Vamos a sacar las funciones activas
        foreach ($server['allowed_functions'] as $funcion) {
            if($count == 1) $funcionesActivas .= $funcion;
            else $funcionesActivas .= '<br>' . $funcion;
            $count++;
        }
        //Creamos un enlace para poder editar el servidor.
        $enlaceEditar = l(t('Edit'), 'admin/config/services/web_server_config/' . $nombre);
        $rows[] = array($nombre, $server['server'], $funcionesActivas, $enlaceEditar);
    }
  
  return theme('table', array('header' => $header, 'rows' => $rows));
}

