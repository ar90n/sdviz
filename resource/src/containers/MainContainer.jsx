import React from 'react';
import { connect } from 'react-redux';
import { setValue, syncValue } from '../actions/ElementActionCreators';
import { connectSuccess, connectFailed } from '../actions/ConnectionActionCreators';

import Main from '../componenets/Main';

const mapStateToProps = ( state ) => {
    return state
};

const mapDispatchToProps = ( dispatch ) => {
    return {
        actions: {
            setValue: function( ws,value ) {
                return dispatch( setValue( ws, value ) );
            },
            syncValue: function( res ) {
                return dispatch( syncValue( res ) );
            },
            connectSuccess() {
                return dispatch( connectSuccess() );
            },
            connectFailed( message ) {
                return dispatch( connectFailed( message ) );
            }
        }
    };
};

const MainContainer = connect(
    mapStateToProps,
    mapDispatchToProps
)( Main );

export default MainContainer;
