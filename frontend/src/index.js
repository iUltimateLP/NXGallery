/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

import React from "react";
import ReactDOM from "react-dom";

// Material UI imports
import {
    colors,
    CssBaseline,
    ThemeProvider,
    Typography,
    Container,
    createTheme,
    Button,
    Grid,
    Paper,
    Icon,
    CircularProgress,
    Dialog,
    DialogContent,
    DialogActions,
    TableContainer,
    Table,
    TableBody,
    TableRow,
    TableCell
} from "@material-ui/core";
import { Pagination } from "@material-ui/lab";

// Set the IP of your switch so it can connect in dev mode
const DEV_IP = "http://192.168.100.25:1234";
const getBackendURL = () => {
    const DEV_MODE = process.env.NODE_ENV === "development";
    return DEV_MODE ? DEV_IP : "";
};

// Create a light and dark material UI theme
const lightTheme = createTheme({
    palette: {
        type: "light",
        primary: {
            main: "#04c7c3",
        },
        secondary: {
            main: "#19857b",
        },
        error: {
            main: colors.red.A400,
        },
        background: {
            default: "#ebebeb",
        },
    }
});

const darkTheme = createTheme({
    palette: {
        type: "dark",
        primary: {
            main: "#50f4dc",
        },
        secondary: {
            main: "#19857b",
        },
        error: {
            main: colors.red.A400,
        },
        background: {
            default: "#2d2d2d",
        }
    }
});

var isSM = window.matchMedia(lightTheme.breakpoints.down('sm').replace("@media ", "")).matches;

// Component for one gallery content
class GalleryItem extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            item: props.item,
            isVideo: props.item.type === "video",
            dialogOpen: false
        };

        this.imageClicked = this.imageClicked.bind(this);
        this.dialogClose = this.dialogClose.bind(this);
    }

    imageClicked() {
        this.setState({
            dialogOpen: true
        });
    }

    dialogClose() {
        this.setState({
            dialogOpen: false
        });
    }

    getDateString() {
        var date = new Date(this.state.item.takenAt * 1000);
        var string = date.toLocaleDateString() + " " + date.toLocaleTimeString();

        return string;
    }

    // https://stackoverflow.com/a/20732091/5028730
    humanFileSize(size) {
        var i = Math.floor( Math.log(size) / Math.log(1024) );
        return ( size / Math.pow(1024, i) ).toFixed(2) * 1 + ' ' + ['B', 'KB', 'MB', 'GB', 'TB'][i];
    };

    render() {
        // Decide whether to show a video or an image element
        let viewElement;
        let viewElementBig;
        const previewURL = getBackendURL() + "/thumbnail?id=" + this.state.item.id;
        const fullURL = getBackendURL() + "/file?id=" + this.state.item.id;
        if (this.state.isVideo) {
            viewElement = <video src={fullURL} controls preload={"none"} poster={previewURL}></video>
            viewElementBig = <video src={fullURL} controls preload={"none"} className={"gallery-content-big"} poster={previewURL}></video>;
        } else {
            viewElement = <img src={previewURL} alt=""></img>
            viewElementBig = <a href={fullURL} target="_blank" rel="noreferrer"><img src={fullURL} className={"gallery-content-big"} alt=""></img></a>;
        }

        return (
            <Grid item xs={isSM ? 12 : 4}>
                <Paper onClick={this.imageClicked} elevation={2} className={"grid-cell"}>
                    {viewElement}
                </Paper>

                <Dialog fullScreen={isSM} open={this.state.dialogOpen} onClose={this.dialogClose} aria-labelledby="responsive-dialog-title">
                    <DialogContent>
                        {viewElementBig}
                        <div className={"gallery-content-bar"}>
                            <a download={this.state.item.fileName} href={fullURL}><Button color="primary"><i className={"fas fa-download"}></i> Download</Button></a>
                        </div>
                        <TableContainer>
                            <Table>
                                <TableBody>
                                    <TableRow>
                                        <TableCell><b>Type</b></TableCell>
                                        <TableCell align="right">{this.state.item.type === "video" ? "Video" : "Screenshot"}</TableCell>
                                    </TableRow>
                                    <TableRow>
                                        <TableCell><b>Taken at</b></TableCell>
                                        <TableCell align="right">{this.getDateString()}</TableCell>
                                    </TableRow>
                                    <TableRow>
                                        <TableCell><b>Game</b></TableCell>
                                        <TableCell align="right">{this.state.item.game}</TableCell>
                                    </TableRow>
                                    <TableRow>
                                        <TableCell><b>Stored at</b></TableCell>
                                        <TableCell align="right">{this.state.item.storedAt === "sd" ? "SD Card" : "Internal Storage"}</TableCell>
                                    </TableRow>
                                    <TableRow>
                                        <TableCell><b>Size</b></TableCell>
                                        <TableCell align="right">{this.humanFileSize(this.state.item.fileSize)}</TableCell>
                                    </TableRow>
                                </TableBody>
                            </Table>
                        </TableContainer>
                    </DialogContent>
                    <DialogActions>
                        <Button color="primary" onClick={this.dialogClose}>Close</Button>
                    </DialogActions>
                </Dialog>
            </Grid>
        );
    }
}

// Component for the whole web app
class App extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            galleryContent: [],
            currentPage: 1,
            maxPages: 1,
            currentTheme: "light",
            stats: {
                numScreenshots: 0,
                numVideos: 0,
                indexTime: 0
            },
            error: false
        };
    }

    componentDidMount() {
        // Fetch the gallery directly at the beginning
        this.fetchGallery();
    }

    fetchGallery() {
        fetch(getBackendURL() + "/gallery?page=" + this.state.currentPage)
        .then(res => res.json())
        .then((result) => {
            this.setState({
                galleryContent: result.gallery,
                maxPages: result.pages,
                currentTheme: result.theme,
                stats: result.stats,
                error: false
            })
        }, (error) => {
            this.setState({
                error: true
            });
        });
    }

    onPageChange(e, page) {
        this.setState({
            currentPage: page
        }, () => {
            this.fetchGallery();
        });
    }

    render() {
        return (
            <ThemeProvider theme={this.state.currentTheme === "light" ? lightTheme : darkTheme}>
                <CssBaseline/>
                <Container style={{flexGrow: 1, padding: "8px"}}>
                    <Typography variant="overline" color="textPrimary" align="left" style={{padding: "8px"}}><i className={"fas fa-images"}></i> Gallery</Typography>

                    <Grid container spacing={2} justifyContent="center">
                        {this.state.galleryContent.map((value) => (
                            <GalleryItem key={value.takenAt} item={value}/>
                        ))}
                    </Grid>

                    {this.state.error && 
                        <Typography variant="h6" color="error" align="center">Oh no, an error has occurred :(</Typography>
                    }

                    {(this.state.galleryContent.length === 0 && !this.state.error) &&
                        <Container align="center" style={{paddingTop: "20px"}}>
                            <CircularProgress color="primary"></CircularProgress>
                        </Container>
                    }

                    {(this.state.galleryContent.length > 0 && !this.state.error) && 
                        <Container align="center" style={{paddingTop: "20px", paddingBottom: "12px"}}>
                            <Pagination count={this.state.maxPages} className={"pagination"} onChange={(e, page) => this.onPageChange(e, page)}/>
                        </Container>
                    }

                    <Container align="center" className={"footer"}>
                        <Typography variant="overline" color="textSecondary" align="center" style={{paddingBottom: "16px", fontWeight: "100"}}>Indexed {this.state.stats.numScreenshots} photos and {this.state.stats.numVideos} videos in {this.state.stats.indexTime.toPrecision(3)} seconds.</Typography><br/>

                        <Typography variant="overline" color="textSecondary" align="center">Made with</Typography>
                        <Icon className={"heart"}>favorite</Icon>
                        <Typography variant="overline" color="textSecondary" align="center">in Bremen, Germany</Typography><br/>
                        <Typography variant="overline" color="textSecondary" align="center"><a href={"https://github.com/iUltimateLP/NXGallery"} target={"_blank"} rel={"noreferrer"}><i className={`fab fa-github ${this.state.currentTheme}`}></i></a></Typography>
                    </Container>
                </Container>
            </ThemeProvider>
        );
    }
}

ReactDOM.render(
    // Render the app
    <App/>,
    document.querySelector("#app-root")
);
