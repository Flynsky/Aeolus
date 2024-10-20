import dash
from dash import dcc, html
from dash.dependencies import Input, Output
import plotly.graph_objs as go
import numpy as np
import time

# Initialize the app
app = dash.Dash()

# Define the layout
app.layout = html.Div([
    html.H1("Real-Time Sine Wave Plot", style={'textAlign': 'center'}),
    dcc.Graph(id='live-graph', animate=True),  # animate=True for smooth transitions
    dcc.Interval(
        id='interval-component',
        interval=1000,  # Update every second
        n_intervals=0  # Number of intervals since the app started
    )
])

# Define the callback to update the graph
@app.callback(
    Output('live-graph', 'figure'),
    [Input('interval-component', 'n_intervals')]
)
def update_graph_live(n_intervals):
    # Generate a sine wave based on time (n_intervals is used as time steps)
    x_data = np.linspace(0, 2 * np.pi, 100)
    y_data = np.sin(x_data + n_intervals / 10.0)  # Adjusting the phase over time
    
    # Create the plotly graph object
    data = go.Scatter(
        x=x_data,
        y=y_data,
        mode='lines',  # Line graph
        name='Sine Wave'
    )
    
    # Define the layout of the graph
    layout = go.Layout(
        xaxis=dict(range=[0, 2 * np.pi]),
        yaxis=dict(range=[-1, 1]),
        title='Live Sine Wave'
    )
    
    # Return the figure for live updating
    return {'data': [data], 'layout': layout}

# Run the app
if __name__ == '__main__':
    app.run_server(debug=True)
