import {combineReducers} from 'redux'
import elements from './Elements'
import connection from './Connection'

const reducers = combineReducers({
    elements,
    connection
});

export default reducers;
