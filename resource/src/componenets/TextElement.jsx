import React from 'react';
import ElementComponent from './ElementComponent';

class TextElement extends ElementComponent {
    static get TYPE() {
        return 0;
    }

    constructor( props, context ) {
        super( props, context );
    }

    render() {
        return ( <p> { this.value } </p>);
    }
};

export default TextElement;
