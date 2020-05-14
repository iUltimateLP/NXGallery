/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

const {
    colors,
    CssBaseline,
    ThemeProvider,
    Typography,
    Container,
    makeStyles,
    createMuiTheme,
    Box,
    SvgIcon,
    Link,
    Button,
    Grid,
    Paper,
    GridList,
    GridListTile,
    Modal
} = MaterialUI;

const theme = createMuiTheme({
    palette: {
      primary: {
        main: '#50f4dc', // 04c7c3
      },
      secondary: {
        main: '#19857b',
      },
      error: {
        main: colors.red.A400,
      },
      background: {
        default: '#ebebeb',
      },
    },
});

function App() {
    return (
        <Container style={{flexGrow: 1, padding: "8px"}}>
            <Typography variant="h2" color="textPrimary" align="center">NXGallery</Typography>
            <Typography variant="h6" color="textSecondary" align="center" style={{paddingBottom: "16px"}}>Browse your Nintendo Switch album with ease!</Typography>

            <Grid container spacing={2} justify="center">
                {[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10].map((value) => (
                    <Grid key={value} item xs={4}>
                        <Paper onClick={buttonClicked()} elevation={2} className={"grid-cell"} key={value}>
                            <img src="assets/plhd.jpg"></img>
                        </Paper>
                    </Grid>
                ))}
            </Grid>
            
        </Container>
    );
}

ReactDOM.render(
    <ThemeProvider theme={theme}>
        <CssBaseline />
        <App />
    </ThemeProvider>,
    document.querySelector("#app-root")
);

function buttonClicked() {

}
