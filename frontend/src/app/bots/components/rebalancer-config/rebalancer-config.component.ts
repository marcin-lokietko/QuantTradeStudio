import { CommonModule } from '@angular/common';
import { ChangeDetectorRef, Component, ViewChild } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { MatButtonModule } from '@angular/material/button';
import { MatCheckboxModule } from '@angular/material/checkbox';
import { MatDialog, MatDialogModule } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatMenuModule } from '@angular/material/menu';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatSelectModule } from '@angular/material/select';
import { MatSort, MatSortModule } from '@angular/material/sort';
import { MatTableDataSource, MatTableModule } from '@angular/material/table';
import { BotName } from '@app/bots/bot-name-enum';
import { AddAssetAndNumberDialog } from '@app/bots/components/add-asset-and-number-dialog/add-asset-and-number-dialog.component';
import { BacktestDialog } from '@app/bots/components/backtest-dialog/backtest-dialog.component';
import { DataService, RequestResult } from '@app/services/data.service';
import { NotificationService } from '@app/services/notification.service';
import { NotificationSeverity } from '@app/shared/components/notification/notification-severity-enum';

interface BaseAssetConfig {
  assetSymbol: string;
  expectedShare: number;
}

@Component({
  selector: 'app-rebalancer-config',
  standalone: true,
  imports: [
    CommonModule,
    FormsModule,
    MatButtonModule,
    MatCheckboxModule,
    MatDialogModule,
    MatFormFieldModule,
    MatIconModule,
    MatInputModule,
    MatMenuModule,
    MatProgressSpinnerModule,
    MatSelectModule,
    MatSortModule,
    MatTableModule,
  ],
  templateUrl: './rebalancer-config.component.html',
  styleUrls: ['./rebalancer-config.component.scss'],
})
export class RebalancerConfigComponent {
  public executionPeriodInputFieldLabel = 'Execution period in seconds';

  public executionPeriodInput = '';
  public isExecutedImmediately = false;

  public availableQuoteAssets: string[] = [];
  public selectedQuoteAsset = '';
  public areAvailableQuoteAssetsLoading = true;

  public availableBaseAssets: string[] = [];
  public selectedBaseAssetsConfig: BaseAssetConfig[] = [];
  public areAvailableBaseAssetsLoading = false;

  public displayedSelectedBaseAssetsTableColumns: string[] = ['assetSymbol', 'expectedShare'];

  dataSource = new MatTableDataSource(this.selectedBaseAssetsConfig);
  @ViewChild(MatSort) sort!: MatSort;

  constructor(
    private dataService: DataService,
    private dialog: MatDialog,
    private notificationService: NotificationService,
    private cdr: ChangeDetectorRef,
  ) {}

  ngOnInit(): void {
    this.fetchAvailableQuoteAssets();
  }

  public onQuoteAssetChange() {
    this.fetchAvailableBaseAssets();
  }

  public updateSelectedBaseAssetConfigTable() {
    this.dataSource = new MatTableDataSource(this.selectedBaseAssetsConfig);
    this.dataSource.sort = this.sort;
    this.cdr.markForCheck();
  }

  public get configTitle(): string {
    return 'Rebalancer configuration';
  }

  public fetchAvailableQuoteAssets(): void {
    this.areAvailableQuoteAssetsLoading = true;
    this.dataService.getQuoteAssetsSuitableForRebalancing().then((data) => {
      if (data) {
        this.availableQuoteAssets = data;
        this.areAvailableQuoteAssetsLoading = false;
        this.cdr.markForCheck();
      }
    });
  }

  public fetchAvailableBaseAssets(): void {
    this.areAvailableBaseAssetsLoading = true;

    this.dataService.getAvailableBaseAssets(this.selectedQuoteAsset).then((data) => {
      if (data) {
        this.availableBaseAssets = data;
        this.areAvailableBaseAssetsLoading = false;
        this.cdr.markForCheck();
      }
    });
  }

  public deleteSelectedBaseAssetConfig(elementToRemove: BaseAssetConfig) {
    this.selectedBaseAssetsConfig = this.selectedBaseAssetsConfig.filter((elem: BaseAssetConfig) => {
      return elem != elementToRemove;
    });
    this.updateSelectedBaseAssetConfigTable();
  }

  public get isQuoteAssetSelected(): boolean {
    return this.selectedQuoteAsset != '';
  }

  public addBaseAsset(): void {
    const dialogRef = this.dialog.open(AddAssetAndNumberDialog, {
      width: '60vw',
      data: {
        title: 'Add base asset share',
        inputFieldLabel: 'Base asset share',
        assets: this.availableBaseAssets.filter((elem: string) => {
          return (
            this.selectedBaseAssetsConfig.findIndex((config) => {
              return config.assetSymbol === elem;
            }) === -1
          );
        }),
      },
    });

    dialogRef.afterClosed().subscribe((result) => {
      console.log('Dialog closed with result:', result);
      if (result) {
        this.selectedBaseAssetsConfig.push({ assetSymbol: result.assetSymbol, expectedShare: Number(result.number) });
        this.updateSelectedBaseAssetConfigTable();
      }
    });

    this.updateSelectedBaseAssetConfigTable();
  }

  public get baseAssetsTotalShare(): number {
    return this.selectedBaseAssetsConfig.reduce((accumulator: number, currentValue: BaseAssetConfig) => {
      return accumulator + currentValue.expectedShare;
    }, 0);
  }

  public get isConfigValid(): boolean {
    return (
      this.executionPeriodInput !== '' &&
      this.selectedQuoteAsset !== '' &&
      this.selectedBaseAssetsConfig.length >= 2 &&
      this.baseAssetsTotalShare === 100
    );
  }

  public get baseAssetConfigSummary(): string {
    if (this.executionPeriodInput === '') {
      return 'Execution period is needed to launch the bot';
    }
    if (this.selectedBaseAssetsConfig.length < 2) {
      return 'At least two base assets are needed to launch the bot';
    }
    let msg = 'Shares of added base assets sum up to ' + this.baseAssetsTotalShare + ' percent.';
    if (this.baseAssetsTotalShare === 100) {
      msg += ' The bot can be launched.';
    } else {
      msg += ' The total share must be equal to 100 percent.';
      return msg;
    }
    if (!this.isConfigValid) {
      return 'Config is invalid; unexpected error occurred';
    }
    return msg;
  }

  public startBot(): void {
    this.dataService.startBot(this.buildBotParams()).then((result) => {
      if (result === RequestResult.Success) {
        console.log('Bot started successfully');
      } else if (result === RequestResult.Fail) {
        this.notificationService.show('Failed to launch the bot', 3000, NotificationSeverity.Error);
      }
    });
  }

  public openBacktestDialog(): void {
    const botParams = this.buildBotParams();
    let availableInitialAssets = botParams.baseAssetShares.map((config) => config.assetSymbol);
    availableInitialAssets.push(botParams.quoteAsset);

    const dialogRef = this.dialog.open(BacktestDialog, {
      width: '60vw',
      data: {
        botParams,
        availableInitialAssets,
      },
    });

    dialogRef.afterClosed().subscribe((result) => {
      console.log('Dialog closed with result:', result);
    });
  }

  private buildBotParams() {
    return {
      botName: BotName.Rebalancer,
      executionPeriod: Number(this.executionPeriodInput),
      isExecutedImmediately: this.isExecutedImmediately,
      quoteAsset: this.selectedQuoteAsset,
      baseAssetShares: this.selectedBaseAssetsConfig,
    };
  }
}
