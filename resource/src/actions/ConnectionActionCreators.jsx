import { WS_CONNECTED, WS_ERROR } from '../constants/ActionTypes';

export function connectSuccess() {
  return {
    type: WS_CONNECTED,
    payload: null
  };
}

export function connectFailed(errorMessage) {
  return {
    type: WS_ERROR,
    payload: errorMessage
  };
}
