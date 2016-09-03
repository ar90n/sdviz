import React from 'react';
import AppBar from 'material-ui/AppBar';
import * as Resources from '../constants/Resources';
import Loader from 'react-loader';
import ContainerElement from './ContainerElement';
import msgpack from 'msgpack-lite';

class Main extends React.Component {
    constructor( props ) {
        super( props );
        this.elements = props.elements;
        this.connection = props.connection;
        this.actions = props.actions;
        this.ws = null;
    }

    componentDidMount() {
        const config_url = `http://${window.location.host}/config`;
        fetch( config_url )
            .then( res => res.json() )
            .then( res => {
                if(!!this.ws) {
                    this.ws.close();
                }

                const ws_url = `ws://${window.location.hostname}:${res.ws_port}`;
                this.ws = new WebSocket( ws_url );
                this.ws.binaryType = 'arraybuffer';
                this.ws.onopen = (e) => this.actions.connectSuccess();
                this.ws.onerror = (e) => this.actions.connectError( msgpack.decode( new Uint8Array( e.data ) ) );
                this.ws.onmessage = (e) => this.actions.syncValue( msgpack.decode( new Uint8Array( e.data ) ) );
            });
    }

    componentWillUpdate( nextProps ) {
        this.elements = nextProps.elements;
        this.connection = nextProps.connection;
        this.actions = nextProps.actions;
    }

    renderMain( is_loaded ) {
        if( !is_loaded )
        {
            return;
        }

        const page_element = this.elements[ Resources.PAGE_ID ];
        const components = <ContainerElement
                                style={this.style}
                                id={Resources.PAGE_ID}
                                value={page_element.value}
                                param={page_element.param}
                                version={page_element.version}
                                setValue={this.actions.setValue}
                                elements={this.elements}
                                ws={this.ws}
                                with_card={false} />;
        return <div style={styles.main}> { components } </div>;
    }

    render() {
        const is_loaded = this.connection.is_connected && !!this.elements[ Resources.PAGE_ID ];
        return (
            <Loader loaded={is_loaded} >
                { this.renderMain( is_loaded ) }
            </Loader>
        )
    }
}

const styles = {
    main: {
        padding: '10px 0px',
    }
};

export default Main;
