import {createStore, applyMiddleware, compose} from 'redux';
import thunk from 'redux-thunk';
import reducers from '../reducers';

const devToolExtensionFunc = window.devToolsExtension ? window.devToolsExtension() : f => f;
const createStoreWithMiddleware = compose( applyMiddleware(thunk), devToolExtensionFunc )(createStore);

function configureStore( initialState ) {
    return createStoreWithMiddleware( reducers, initialState );
}

export default configureStore;
