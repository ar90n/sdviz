import { SET_VALUE, SYNC_VALUE } from '../constants/ActionTypes'

export function setValue( ws, obj ) {
    return ( dispatch ) => {
        ws.send( JSON.stringify( obj ) );
    };
}

export function syncValue( res ) {
    const payload = {}
    if( !Array.isArray( res ) )
    {
        res = [res];
    }
    res.forEach( ( { id, type, label, value, param, version } ) => {
        payload[id] = { type, label, value, param, version }
    })

    return {
        type: SYNC_VALUE,
        payload: payload
    }
}
