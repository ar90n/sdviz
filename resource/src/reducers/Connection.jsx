import { WS_CONNECTED, WS_ERROR } from '../constants/ActionTypes'

const initialState = {
    is_connected: false
};

function connection( state = initialState, action ) {
    const { type, payload } = action;

    switch( type ) {
        case WS_CONNECTED:
            return Object.assign( {}, state, { is_connected: true } );
        case WS_ERROR:
            return Object.assign( {}, state, { is_connected: false } );
        default:
            return state;
    }
}

export default connection;
