<?php

/**
 * Funcione que realiza una consulta a base de datos para obtener el email del cliente pasando el nid del vial.
 * @param type int
 * @return string 'email del cliente'
 */
function conectorSegeco2_getEmailDelCliente($idVial) {
    $resultado = false;
    $query = db_select('node', 'cp');
    $query->leftJoin('field_data_field_cartapago_vial', 'fcv', 'cp.nid = fcv.entity_id');    
    $query->leftJoin('node', 'v', 'fcv.field_cartapago_vial_target_id = v.nid');
    $query->leftJoin('field_data_field_cartapago_propietario', 'fcp', 'cp.nid = fcp.entity_id');        
    $query->leftJoin('node', 'p', 'fcp.field_cartapago_propietario_target_id = p.nid');
    $query->leftJoin('field_data_field_cartapago_propietario', 'fcp', 'cp.nid = fcp.entity_id');        
    $query->leftJoin('field_data_field_propietario_email', 'e', 'p.nid = e.entity_id');     
    $result = $query
            ->fields('e', array('field_propietario_email_value'))
            ->condition('v.nid', $idVial, '=')
            ->execute();
    if ($result) {
        while ($record = $result->fetchAssoc()) {
            return $record['field_propietario_email_value'];
        }
    }
    return 0;
}

/**
 * funcion que verifica si un vial está asociado a una solicitud
 * Devuelve false si está lleno
 * devuelve true si está vacio
 */
function conectorSegeco2_checkVialVacio($idVial) {
    $resultado = false;
    $query = db_select('node', 'n');
    $query->join('field_data_field_vial_solicitud', 'fvs', 'n.nid = fvs.entity_id');
    $result = $query
            ->fields('n', array('nid'))
            ->condition('nid', $idVial, '=')
            ->execute();
    if ($result) {
        while ($record = $result->fetchAssoc()) {
            $resultado = FALSE;
        }
    } else {
        $resultado = TRUE;
    }
    return $resultado;
}

/**
 *  * función que devuelve el nombre del usuario pasandole el id
 */
function conectorSegeco2_getNodeById($id) {
    $query = db_select('node', 'n');
    $result = $query
            ->fields('n', array('title'))
            ->condition('nid', $id, '=')
            ->execute();
    while ($record = $result->fetchAssoc()) {
        return $record['title'];
    }
    return 0;
}

/**
 * función que devuelve un nodo pasandole el título, 
 */
function conectorSegeco2_getNodeByTitle($title) {
    $query = db_select('node', 'n');
    $result = $query
            ->fields('n', array('nid'))
            ->condition('title', $title, '=')
            ->execute();
    while ($record = $result->fetchAssoc()) {
        return $record['nid'];
    }
    return 0;
}

/**
 * función que devuelve un nodo pasandole el título, 
 */
function conectorSegeco2_getUserByName($name) {
    $query = db_select('users', 'u');
    $result = $query
            ->fields('u', array('uid'))
            ->condition('name', $name, '=')
            ->execute();
    while ($record = $result->fetchAssoc()) {
        return $record['uid'];
    }
    return 0;
}

/**
 *  * función que devuelve el nombre del usuario pasandole el id
 */
function conectorSegeco2_getUserById($id) {
    $query = db_select('users', 'u');
    $result = $query
            ->fields('u', array('name'))
            ->condition('uid', $id, '=')
            ->execute();
    while ($record = $result->fetchAssoc()) {
        return $record['name'];
    }
    return 0;
}

/**
 * función para cambiar datos de forma masiva
 */
function conectorSegeco2_setdatosMasivo() {
    $nidViales = array();

    $query = db_select('node', 'n');
    $query->join('field_data_field_vial_estado', 'estado', 'n.nid = estado.entity_id');
    $result = $query
            ->fields('n', array('nid'))
            /* ->condition('type', 'solicitud_genotipado', '=')
              ->condition('field_vial_estado_value', 1, '=') */
            ->execute();
    while ($record = $result->fetchAssoc()) {
        $nidViales[] = $record['nid'];
    }

    foreach ($nidViales as $key => $idVial) {
        $n = node_load($idVial);
        $estado = $n->field_vial_estado['und'][0]['value'];
        //Vamos a cambiarle los estados
        switch ($estado) {
            case 11:
                $n->field_vial_estado['und'][0]['value'] = 2;
                node_save($n);
                echo "vial $n->title modificado";
                break;
            case 12:
                $n->field_vial_estado['und'][0]['value'] = 3;
                node_save($n);
                echo "vial $n->title modificado";
                break;
            case 13:
                $n->field_vial_estado['und'][0]['value'] = 4;
                node_save($n);
                echo "vial $n->title modificado";
                break;
            case 14:
                $n->field_vial_estado['und'][0]['value'] = 9;
                node_save($n);
                echo "vial $n->title modificado";
                break;
        }
    }
}
