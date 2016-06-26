import React from 'react';
import { Provider } from 'react-redux'
import ReactDOM from 'react-dom';
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider';
import configureStore from './store/configure-store';
import MainContainer from './containers/MainContainer';

const rootEl = document.getElementById('root')

const store = configureStore();

ReactDOM.render(
  <MuiThemeProvider>
      <Provider store={store} >
          <MainContainer />
      </Provider>
  </MuiThemeProvider>,
  rootEl
);
