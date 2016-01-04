<?php




function get_item_from_buffer() {
    $queue = DrupalQueue::get('BBIVialLleno');
    //si hay elementos los devolvemos.
    $item = $queue->claimItem();  
    if($item) return $item;
    else return FALSE;
}

function insert_item_into_buffer($node) {
    if($node->type == 'solicitud_genotipado' || $node->type == 'solicitud_de_contraste') {
        //Ahora cogemos los datos..
        $vial['titulo']            = isset($node->field_solicitud_gen_vial['und'][0]['target_id']) ? conectorSegeco2_getNodeById($node->field_solicitud_gen_vial['und'][0]['target_id']) : NULL;
        $vial['fechaVialLleno']    = date('j-m-Y', $node->created);  
        $vial['tituloSolicitud']   = $node->title; 
        //Los campos de la chapa no están todavía
        $vial['tipoChapa']         = NULL;    
        $vial['nombrePerro']       = NULL;    
        $vial['digitoControl']     = NULL;   
        $vial['crearChapa']        = NULL;
        
        //Creamos el item para que cuando la tienda venga los coja.
        $item               = new stdClass();
        $item->data         = $vial;
        $item->operaciones  = array(11, 100);//update vial y dejarlo en cola para segcan
        $queue = DrupalQueue::get('BBIVialLleno');
        $queue->createItem($item);          
    }
}

function remove_item_from_buffer($item_id) {
    $item = new stdClass();
    $item->item_id = $item_id;    
    $queue = DrupalQueue::get('BBIVialLleno');
    $queue->deleteItem($item);
}

function insert_item_error_buffer($item) {
    $queue = DrupalQueue::get('BBIVialLleno_error');
    $queue->createItem($item);
}

function write_element_from_buffer() {
    //Aquí hay que hacer un switch para ir viendo las diferentes operaciones.
    //Las operaciones serán:
    //insertar vial, update vial, insertar chapa, eliminar viales de caja vacía
    $resultado = FALSE;
    $data = $item->data;
    foreach ($data->operaciones as $operacion) {
        $resultado = FALSE;
        switch ($operacion) {
            case 21://Insertar vial
                $vial = $data->vial;
                if(node_load(conectorSegeco2_getNodeByTitle($vial['titulo']))) return false;
                $values = array(
                    'type'      => 'vial',
                    'uid'       => 1,
                    'status'    => 1,
                    'comment'   => 0,
                    'promote'   => 0,
                );
                $entity = entity_create('node', $values);
                $ewrapper = entity_metadata_wrapper('node', $entity);  
                $ewrapper->title->set($vial['titulo']);
                $ewrapper->field_vial_tipo_de_vial->set($vial['tipoDeVial']);    
                $ewrapper->save();    
                if(node_load($ewrapper->getIdentifier())) $resultado = TRUE;
                break;

            case 22://Update vial
                $vial = $data->vial;
                //Cogemos el vial para poder acceder a la solicitud asociada
                if(!conectorSegeco2_getNodeByTitle($vial['titulo'])) return FALSE;
                $v              = node_load(conectorSegeco2_getNodeByTitle($vial['titulo']));  
                $idSolicitud    = $v->field_vial_solicitud['und'][0]['target_id'];  
                if(!$idSolicitud) return FALSE;
                $s              = node_load((int)$idSolicitud);


                //accedo al estado de la solicitud y lo cambio
                $s->field_vial_estado['und'][0]['value'] = (int)$vial['resultado']; 

                //Si es una solicitud de contraste..
                if(isset($s->field_contraste_vial_matching)) {

                    //Le metemos la fecha de informe, hay que meterle una 'T' para que hacepte el formato en la base de datos, solo para el drupal de xativa, en los demás esta bien es tipo datetime, en xativa es date
                    if($vial['fechaInforme']) {
                       $s->field_fecha_contraste_emision['und'][0]['value'] = str_replace(" ", "T", $vial['fechaInforme']['value']);
                    } else {
                        $s->field_fecha_contraste_emision = array();
                    }

                    //Si tiene matching resultado = 1 positivo
                    if($vial['matching']) {
                        $s->field_contraste_vial_matching['und'][0]['target_id'] = conectorSegeco2_getNodeByTitle($vial['matching']);
                        $s->field_perro_resultado['und'][0]['value'] = 1;//positivo
                    }
                    //Si el estado es 4 = no gen
                    if($vial['resultado'] == 4) $s->field_perro_resultado['und'][0]['value'] = 0;//nulo
                    //Si el estado es 3 = ok y no tiene matching
                    if($vial['resultado'] == 3 && !$vial['matching']) $s->field_perro_resultado['und'][0]['value'] = 2;//negativo
                } else {
                    //Si es una solicitud de genotipado, hay que mandar email al dueño para decirle que ya puede recoger la chapa
                    //Vamos a buscar el email del cliente, si no tiene que no haga nada...
                    $emaildelCliente = conectorSegeco2_getEmailDelCliente($v->nid);
                    //Si el propietario tiene email sel le envía.
                    if($emaildelCliente)
                    simple_mail_send('censocanino@ayto-xativa.es', $emaildelCliente, 'Entrega de chapa', 'La chapa de su perro ya esta lista para su recogida, pase por su ayuntamiento en la mayor brevedad.');
                }

                node_save($s);
                if($s->field_vial_estado['und'][0]['value'] != $vial['resultado']) return FALSE;
                return TRUE;
                break;

            case 31://Insertar chapa
                

                break;
            case 32://Update chapa
                $chapa = $data->chapa;
                $ch = node_load(conectorSegeco2_getNodeByTitle($chapa['titulo']));
                $ch->field_fecha_envio_chapa['und'][0]['value'] = $chapa['fechaEnvio'];
                node_save($ch);
                $resultado = TRUE;
                break;
            case 50://Eliminar viales vacios de una caja
                $tituloVial = $data->vial;
                $nidVial = conectorSegeco2_getNodeByTitle($tituloVial);
                //Nos aseguramos de que no tenga solicitud asociada.
                if(conectorSegeco2_checkVialVacio($nidVial)) {
                    node_delete($nidVial);                    
                    $resultado = TRUE;
                }
                break;
        }
    }
    return $resultado;
}





















/**
 * Funcion que elimina los viales vacios de una caja cuando se cierra
 */
function conectorSegeco_eliminarVialesVaciosDeUnaCaja($viales) {
    $vialesVacios = array();
    
    //Vamos a coger los id de los viales que tienen aquí en lebrija
    foreach ($viales as $id => $titulo) {
        $vialesVacios[conectorSegeco2_getNodeByTitle($titulo)] = $titulo;
    }
    
    //Vamos a comprobar que no tengan solicitud asociada, si la tienen lo dejamos y no se elimina
    foreach ($vialesVacios as $idVial => $titulo) {
        if(conectorSegeco2_checkVialVacio($idVial)) {
            //Si está lleno lo eliminamos del array
            unset($vialesVacios[$idVial]);
        }
    }
    
    //Ahora ya podemos eliminar los viales seguros de que están vacios
    foreach ($vialesVacios as $idVial => $titulo) {
         node_delete($idVial);
    }
    return 'se han eliminado los viales correctamente';
}

/**
 * Funcion que crea una chapa en el segeco
 * @param array $chapa
 * @return boolean
 */
function conectorSegeco_setChapa($chapa) {
    $ch = node_load(conectorSegeco2_getNodeByTitle($chapa['titulo']));
    $ch->field_fecha_envio_chapa['und'][0]['value'] = $chapa['fechaEnvio'];
    node_save($ch);
    return TRUE;
}