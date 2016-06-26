import { SET_VALUE, SYNC_VALUE } from '../constants/ActionTypes'

const initialState = {
};

function elements( state = initialState, action ) {
    const { type, payload } = action;

    switch( type ) {
        case SET_VALUE:
        case SYNC_VALUE:
            const new_state = Object.assign( {}, state );
            for( const id in payload ) {
                new_state[id] = Object.assign( {}, new_state[id], payload[id] );
            }

            return new_state;
        default:
            return state;
    }
}

export default elements;
