import dash
from dash import dcc, html, Input, Output, State
import plotly.graph_objs as go
from datetime import datetime

# Initialize the Dash app
app = dash.Dash(__name__)

# Initialize variables
x_data = []
y_data = []
console_output = []

# App layout
app.layout = html.Div([
    html.H1("Simple Dash GUI with Plotly"),
    
    # Input for discrete values
    html.Div([
        html.Label("Enter Value:"),
        dcc.Input(id="input-value", type="number", value=0, step=1),
        html.Button("Add Value", id="submit-button", n_clicks=0),
    ], style={"display": "inline-block", "margin-right": "20px"}),
    
    # Console display
    html.Div([
        html.H3("Console"),
        html.Div(id="console", style={"whiteSpace": "pre-line", "border": "1px solid black", "padding": "10px", "height": "200px", "overflowY": "scroll"}),
    ], style={"width": "300px", "display": "inline-block", "verticalAlign": "top"}),
    
    # Plot
    html.Div([
        dcc.Graph(id="live-graph"),
    ], style={"width": "600px", "display": "inline-block", "margin-left": "20px"})
])

# Callback to update plot and console
@app.callback(
    [Output("live-graph", "figure"), Output("console", "children")],
    [Input("submit-button", "n_clicks")],
    [State("input-value", "value")]
)

def update_graph(n_clicks, new_value):
    if n_clicks > 0:
        # Add the new value to the data
        x_data.append(len(x_data))
        y_data.append(new_value)
        
        # Log to the console
        console_output.append(f"{datetime.now().strftime('%H:%M:%S')}: Added value {new_value}")
        
        # Update the plot figure
        figure = go.Figure(
            data=[go.Scatter(x=x_data, y=y_data, mode="lines+markers", marker=dict(color="blue"))],
            layout=go.Layout(title="Live Data Plot", xaxis=dict(title="Index"), yaxis=dict(title="Value"))
        )
        
        # Return the updated plot and console log
        return figure, "\n".join(console_output)

    # Initial blank plot
    figure = go.Figure(
        data=[go.Scatter(x=[], y=[], mode="lines+markers", marker=dict(color="blue"))],
        layout=go.Layout(title="Live Data Plot", xaxis=dict(title="Index"), yaxis=dict(title="Value"))
    )
    return figure, "\n".join(console_output)

if __name__ == "__main__":
    app.run_server(debug=True)
